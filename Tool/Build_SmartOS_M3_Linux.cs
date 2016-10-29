using System;
using System.Collections;
using System.Diagnostics;
using System.Reflection;
using System.Text;
using System.Linq;
using System.IO;
using System.Collections.Generic;
using Microsoft.Win32;
using NewLife.Log;

namespace NewLife.Reflection
{
    public class ScriptEngine
    {
        static void Main()
        {
            var build = new Builder();
            build.Init();
			build.Cortex = 3;
			build.Defines.Add("STM32F1");
            build.AddFiles("..\\Core");
            build.AddFiles("..\\Kernel");
            build.AddFiles("..\\Device");
            build.AddFiles("..\\", "*.c;*.cpp", false);
            build.AddFiles("..\\Security", "*.cpp");
            build.AddFiles("..\\Board");
            build.AddFiles("..\\Storage");
            build.AddFiles("..\\App");
            build.AddFiles("..\\Drivers");
            build.AddFiles("..\\Net");
            build.AddFiles("..\\Test");
            build.AddFiles("..\\TinyIP", "*.c;*.cpp", false, "HttpClient");
            build.AddFiles("..\\Message");
            build.AddFiles("..\\TinyNet");
            build.AddFiles("..\\TokenNet");
			build.Libs.Clear();
			//build.ExtCompiles.Add("--enum_is_int");
			//build.ExtCompiles.Add("--signed_chars");
			//build.ExtCompiles.Add("--wchar32");
			build.Linux = true;
            build.CompileAll();
            build.BuildLib("..\\SmartOS_M3");

			build.Debug = true;
            build.CompileAll();
            build.BuildLib("..\\SmartOS_M3");

			/*build.Debug = false;
			build.Tiny = true;
            build.CompileAll();
            build.BuildLib("..\\SmartOS_M3");*/
        }
    }
}
	//include=MDK.cs
