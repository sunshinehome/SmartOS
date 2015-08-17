using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using Microsoft.Win32;

//【XScript】C#脚本引擎v1.8源码（2015/2/9更新）
//http://www.newlifex.com/showtopic-369.aspx

/*
 * 脚本功能：
 * 1，向固件写入编译时间、编译机器等编译信息
 * 2，生成bin文件，显示固件大小
 * 3，直接双击使用时，设置项目编译后脚本为本脚本
 * 4，清理无用垃圾文件
 * 5，从SmartOS目录更新脚本自己
 */

namespace NewLife.Reflection
{
    public class ScriptEngine
    {
        static Int32 Main(String[] args)
        {
            // 双击启动有两个参数，第一个是脚本自身，第二个是/NoLogo
            if (args == null || args.Length <= 2)
            {
                SetProjectAfterMake();
            }
            else
            {
                var axf = GetAxf(args);
                if (!String.IsNullOrEmpty(axf))
                {
                    // 修改编译信息
                    if (WriteBuildInfo(axf)) MakeBin(axf);
                }
            }

            // 清理垃圾文件
            Clear();

            // 更新脚本自己
            //UpdateSelf();

			// 编译SmartOS
			var path = ".".GetFullPath().ToUpper();
			if(path.Contains("STM32F0"))
				"XScript".Run("..\\..\\SmartOS\\Tool\\Build_SmartOS_F0.cs /NoLogo /NoStop");
			else if(path.Contains("STM32F1"))
				"XScript".Run("..\\SmartOS\\Tool\\Build_SmartOS_F1.cs /NoLogo /NoStop");
			else if(path.Contains("STM32F4"))
				"XScript".Run("..\\SmartOS\\Tool\\Build_SmartOS_F4.cs /NoLogo /NoStop");

			"完成".SpeakAsync();
			System.Threading.Thread.Sleep(250);

            return 0;
        }

        /// <summary>获取项目文件名</summary>
        /// <returns></returns>
        static String GetProjectFile()
        {
            var fs = Directory.GetFiles(".".GetFullPath(), "*.uvprojx");
            if (fs.Length == 0) Directory.GetFiles(".".GetFullPath(), "*.uvproj");
            if (fs.Length == 0)
            {
                Console.WriteLine("找不到项目文件！");
                return null;
            }
            if (fs.Length > 1)
            {
                Console.WriteLine("找到项目文件{0}个，无法定夺采用哪一个！", fs.Length);
                return null;
            }

            return Path.GetFileName(fs[0]);
        }

        /// <summary>设置项目的编译后脚本</summary>
        static void SetProjectAfterMake()
        {
            Console.WriteLine("设置项目的编译脚本");

            /*
             * 找到项目文件
             * 查找<AfterMake>，开始处理
             * 设置RunUserProg1为1
             * 设置UserProg1Name为XScript.exe Build.cs /NoLogo /NoTime /NoStop
             * 循环查找<AfterMake>，连续处理
             */

            var file = GetProjectFile();
            Console.WriteLine("加载项目：{0}", file);
            file = file.GetFullPath();

            var doc = new XmlDocument();
            doc.Load(file);

            var nodes = doc.DocumentElement.SelectNodes("//AfterMake");
            Console.WriteLine("发现{0}个编译目标", nodes.Count);
            var flag = false;
            foreach (XmlNode node in nodes)
            {
                var xn = node.SelectSingleNode("../../../TargetName");
                Console.WriteLine("编译目标：{0}", xn.InnerText);

                xn = node.SelectSingleNode("RunUserProg1");
                xn.InnerText = "1";
                xn = node.SelectSingleNode("UserProg1Name");

                var bat = "XScript.exe Build.cs /NoLogo /NoTime /NoStop";
                if (xn.InnerText != bat)
                {
                    xn.InnerText = bat;
                    flag = true;
                }
            }

            if (flag)
            {
                Console.WriteLine("保存修改！");
                //doc.Save(file);
                var set = new XmlWriterSettings();
                set.Indent = true;
                // Keil实在烂，XML文件头指明utf-8编码，却不能添加BOM头
                set.Encoding = new UTF8Encoding(false);
                using (var writer = XmlWriter.Create(file, set))
                {
                    doc.Save(writer);
                }
            }
        }

        static String GetAxf(String[] args)
        {
            var axf = args.FirstOrDefault(e => e.EndsWithIgnoreCase(".axf"));
            if (!String.IsNullOrEmpty(axf)) return axf.GetFullPath();

            // 搜索所有axf文件，找到最新的那一个
            var fis = Directory.GetFiles(".", "*.axf", SearchOption.AllDirectories);
            if (fis != null && fis.Length > 0)
            {
                // 按照修改时间降序的第一个
                return fis.OrderByDescending(e => e.AsFile().LastWriteTime).First().GetFullPath();
            }

            Console.WriteLine("未能从参数中找到输出文件.axf，请在命令行中使用参数#L");
            return null;
        }

        /// <summary>写入编译信息</summary>
        /// <param name="axf"></param>
        /// <returns></returns>
        static Boolean WriteBuildInfo(String axf)
        {
            // 修改编译时间
            var ft = "yyyy-MM-dd HH:mm:ss";
            var sys = axf.GetFullPath();
            if (!File.Exists(sys)) return false;

            var dt = ft.GetBytes();
            var company = "NewLife_Embedded_Team";
            //var company = "NewLife_Team";
            var name = String.Format("{0}_{1}", Environment.MachineName, Environment.UserName);
            if (name.GetBytes().Length > company.Length)
                name = name.CutBinary(company.Length);

            var rs = false;
            // 查找时间字符串，写入真实时间
            using (var fs = File.Open(sys, FileMode.Open, FileAccess.ReadWrite))
            {
                if (fs.IndexOf(dt) > 0)
                {
                    fs.Position -= dt.Length;
                    var now = DateTime.Now.ToString(ft);
                    Console.WriteLine("编译时间：{0}", now);
                    fs.Write(now.GetBytes());

                    rs = true;
                }
                fs.Position = 0;
                var ct = company.GetBytes();
                if (fs.IndexOf(ct) > 0)
                {
                    fs.Position -= ct.Length;
                    Console.WriteLine("编译机器：{0}", name);
                    fs.Write(name.GetBytes());
                    // 多写一个0以截断字符串
                    fs.Write((Byte)0);

                    rs = true;
                }
            }

            return rs;
        }

        static String GetKeil()
        {
            var reg = Registry.LocalMachine.OpenSubKey("Software\\Keil\\Products\\MDK");
            if (reg == null) reg = Registry.LocalMachine.OpenSubKey("Software\\Wow6432Node\\Keil\\Products\\MDK");
            if (reg == null) return null;

            return reg.GetValue("Path") + "";
        }

        /// <summary>生产Bin文件</summary>
        static void MakeBin(String axf)
        {
            // 修改成功说明axf文件有更新，需要重新生成bin
            // 必须先找到Keil目录，否则没得玩
            var mdk = GetKeil();
            if (!String.IsNullOrEmpty(mdk) && Directory.Exists(mdk))
            {
                var fromelf = mdk.CombinePath("ARMCC\\bin\\fromelf.exe");
                //var bin = Path.GetFileNameWithoutExtension(axf) + ".bin";
                var prj = Path.GetFileNameWithoutExtension(GetProjectFile());
                if (Path.GetFileNameWithoutExtension(axf).EndsWithIgnoreCase("D"))
                    prj += "D";
                var bin = prj + ".bin";
                var bin2 = bin.GetFullPath();
                //Process.Start(fromelf, String.Format("--bin {0} -o {1}", axf, bin2));
                var p = new Process();
                p.StartInfo.FileName = fromelf;
                p.StartInfo.Arguments = String.Format("--bin {0} -o {1}", axf, bin2);
                //p.StartInfo.CreateNoWindow = false;
                p.StartInfo.UseShellExecute = false;
                p.Start();
                p.WaitForExit(5000);
                var len = bin2.AsFile().Length;
                Console.WriteLine("生成固件：{0} 共{1:n0}字节/{2:n2}KB", bin, len, (Double)len / 1024);
            }
        }

        /// <summary>清理无用文件</summary>
        static void Clear()
        {
            // 清理bak
            // 清理dep
            // 清理 用户名后缀
            // 清理txt/ini

            Console.WriteLine();
            Console.WriteLine("清理无用文件");

            var ss = new String[] { "bak", "dep", "txt", "ini", "htm" };
            var list = new List<String>(ss);
            list.Add(Environment.UserName);

            foreach (var item in list)
            {
                var fs = Directory.GetFiles(".".GetFullPath(), "*." + item);
                if (fs.Length > 0)
                {
                    foreach (var elm in fs)
                    {
                        Console.WriteLine("删除 {0}", elm);
                        try
                        {
                            File.Delete(elm);
                        }
                        catch { }
                    }
                }
            }
        }

        /// <summary>更新脚本自己</summary>
        static void UpdateSelf()
        {
			var deep = 1;
            // 找到SmartOS目录，里面的脚本可用于覆盖自己
            var di = "../SmartOS".GetFullPath();
            if (!Directory.Exists(di)){ deep++; di = "../../SmartOS".GetFullPath();}
            if (!Directory.Exists(di)){ deep++; di = "../../../SmartOS".GetFullPath();}
            if (!Directory.Exists(di)) return;

            var fi = di.CombinePath("Tool/Build.cs");
			switch(deep)
			{
				case 2:fi = di.CombinePath("Tool/Build2.cs");break;
				case 3:fi = di.CombinePath("Tool/Build3.cs");break;
				default: break;
			}

            if (!File.Exists(fi)) return;

            var my = "Build.cs".GetFullPath();
            if (my.AsFile().LastWriteTime >= fi.AsFile().LastWriteTime) return;

            try
            {
                File.Copy(fi, my, true);
            }
            catch { }
        }
    }
}