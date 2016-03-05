﻿#ifndef __String_H__
#define __String_H__

#include "Sys.h"
#include "Type.h"

// 字符串助手，主要用于字符串连接
class StringHelper;

// 字符串
class String : public Object
{
private:
	/*// http://www.artima.com/cppsource/safebool.html
	typedef void (String::*StringIfHelperType)() const;
	void StringIfHelper() const {}*/

public:
	// 字符串默认0长度，容量0x40
	//String(int length = 0) : TArray(length) { }
	//String(char item, int count) : TArray(count) { Set(item, 0, count); }
	// 因为使用外部指针，这里初始化时没必要分配内存造成浪费
	//String(void* str, int len = -1) : TArray(0) { Set(str, len); }
	//String(const void* str, int len = -1) : TArray(0) { Set(str, len); }
	//String(const Array& bs) : TArray(bs.Length()) { Copy(bs, 0); }

	String(const char* cstr = "");
	String(const String& str);
	String(String&& rval);
	String(StringHelper&& rval);
	String(char* str, int length);
	explicit String(char c);
	explicit String(byte value, byte radix = 10);
	explicit String(int value, byte radix = 10);
	explicit String(uint value, byte radix = 10);
	explicit String(Int64 value, byte radix = 10);
	explicit String(ulong value, byte radix = 10);
	explicit String(float value, byte decimalPlaces = 2);
	explicit String(double value, byte decimalPlaces = 2);
	virtual ~String();

	// 内存管理
	bool CheckCapacity(uint size);
	inline uint Length() const { return len; }
	inline char* GetBuffer() const { return buffer; }
	void SetBuffer(const void* str, int length);

	// 为被赋值对象建立一个备份。
	// 如果值为空或无效，或者内存分配失败，字符串将会被标记为无效
	String& operator = (const String& rhs);
	String& operator = (const char* cstr);
	String& operator = (String&& rval);
	String& operator = (StringHelper&& rval);

	// 连接内建类型。如果参数无效则认为连接失败
	bool Concat(const Object& obj);
	bool Concat(const String& str);
	bool Concat(const char* cstr);
	bool Concat(char c);
	bool Concat(byte c, byte radix = 10);
	bool Concat(int num, byte radix = 10);
	bool Concat(uint num, byte radix = 10);
	bool Concat(Int64 num, byte radix = 10);
	bool Concat(ulong num, byte radix = 10);
	bool Concat(float num);
	bool Concat(double num);

	/*template<typename T>
	String& operator += (T rhs)	{Concat(rhs); return (*this);}*/
	String& operator += (const Object& rhs)	{Concat(rhs); return (*this);}
	String& operator += (const String& rhs)	{Concat(rhs); return (*this);}
	String& operator += (const char* cstr)	{Concat(cstr); return (*this);}
	String& operator += (char c)			{Concat(c); return (*this);}
	String& operator += (byte num)			{Concat(num); return (*this);}
	String& operator += (int num)			{Concat(num); return (*this);}
	String& operator += (uint num)			{Concat(num); return (*this);}
	String& operator += (Int64 num)			{Concat(num); return (*this);}
	String& operator += (ulong num)			{Concat(num); return (*this);}
	String& operator += (float num)			{Concat(num); return (*this);}
	String& operator += (double num)		{Concat(num); return (*this);}

	/*template<typename T>
	friend StringHelper& operator + (const StringHelper& lhs, T rhs)
	{
		auto& a = const_cast<StringHelper&>(lhs);
		if (!a.Concat(rhs)) a.release();
		return a;
	}*/
	friend StringHelper& operator + (const StringHelper& lhs, const Object& rhs);
	friend StringHelper& operator + (const StringHelper& lhs, const String& rhs);
	friend StringHelper& operator + (const StringHelper& lhs, const char* cstr);
	friend StringHelper& operator + (const StringHelper& lhs, char c);
	friend StringHelper& operator + (const StringHelper& lhs, byte num);
	friend StringHelper& operator + (const StringHelper& lhs, int num);
	friend StringHelper& operator + (const StringHelper& lhs, uint num);
	friend StringHelper& operator + (const StringHelper& lhs, Int64 num);
	friend StringHelper& operator + (const StringHelper& lhs, ulong num);
	friend StringHelper& operator + (const StringHelper& lhs, float num);
	friend StringHelper& operator + (const StringHelper& lhs, double num);

    //operator bool() const { return len > 0; }
	operator char*() const { return buffer; }
	int CompareTo(const String& s) const;
	bool Equals(const String& s) const;
	bool Equals(const char* cstr) const;
	bool EqualsIgnoreCase(const String& s) const;
	bool operator == (const String& rhs) const {return Equals(rhs);}
	bool operator == (const char* cstr) const {return Equals(cstr);}
	bool operator != (const String& rhs) const {return !Equals(rhs);}
	bool operator != (const char* cstr) const {return !Equals(cstr);}
	bool operator <  (const String& rhs) const;
	bool operator >  (const String& rhs) const;
	bool operator <= (const String& rhs) const;
	bool operator >= (const String& rhs) const;

	// character acccess
	char charAt(unsigned int index) const;
	void setCharAt(unsigned int index, char c);
	char operator [] (unsigned int index) const;
	char& operator [] (unsigned int index);
	void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index=0) const;
	void toCharArray(char *buf, unsigned int bufsize, unsigned int index=0) const
		{getBytes((unsigned char *)buf, bufsize, index);}
	const char* c_str() const { return buffer; }

	// parsing/conversion
	long toInt(void) const;
	float toFloat(void) const;

	// 输出对象的字符串表示方式
	virtual String& ToStr(String& str) const;
	// 输出对象的字符串表示方式
	virtual String ToString() const;
	// 清空已存储数据。
	virtual void Clear();

	/*String& Append(char ch);
	String& Append(const char* str, int len = -1);
	String& Append(int value, int radix = 10, int width = 0);	// 写入整数，第二参数指定宽带，不足时补零
	String& Append(byte bt);		// 十六进制
	String& Append(const ByteArray& bs);	// 十六进制*/

	// 调试输出字符串
	virtual void Show(bool newLine = false) const;

	// 格式化字符串，输出到现有字符串后面。方便我们连续格式化多个字符串
	String& Format(const char* format, ...);

    //String& Concat(const Object& obj);
    //String& Concat(const char* str, int len = -1);

	int IndexOf(const char ch, int startIndex = 0) const;
	int IndexOf(const String& str, int startIndex = 0) const;
	int IndexOf(const char* str, int startIndex = 0) const;
	int LastIndexOf(const char ch, int startIndex = 0) const;
	int LastIndexOf(const String& str, int startIndex = 0) const;
	int LastIndexOf(const char* str, int startIndex = 0) const;
	bool StartsWith(const String& str, int startIndex = 0) const;
	bool StartsWith(const char* str, int startIndex = 0) const;
	bool EndsWith(const String& str) const;
	bool EndsWith(const char* str) const;

	String Substring(int start, int len) const;
	String& TrimStart();
	String& TrimEnd();
	String& Trim();
	String& Replace(char find, char replace);
	String& Replace(const String& find, const String& replace);
	String& Remove(int index);
	String& Remove(int index, int count);
	String& ToLower();
	String& ToUpper();

    /*String& operator+=(const Object& obj);
    //String& operator+=(const char* str);
    friend String& operator+(String& str, const Object& obj);
    friend String& operator+(String& str, const char* str2);
    friend String& operator+(String& str, char ch);
    friend String& operator+(String& str, byte bt);
    friend String& operator+(String& str, int value);

    friend bool operator==(const String& str, const String& str2);
    friend bool operator!=(const String& str, const String& str2);
    friend bool operator==(const String& str, const char* str2);
    friend bool operator!=(const String& str, const char* str2);*/

protected:
	char*	buffer;		// 字符数组
	int		capacity;	// 容量，不包含0结束符
	int		len;		// 字符串长度，不包含0结束符

	char	Arr[0x40];

protected:
	void init();
	void release();
	bool Concat(const char* cstr, uint length);

	String& copy(const char* cstr, uint length);
	void move(String& rhs);
};

//String operator+(const char* str1, const char* str2);
//String operator+(const char* str, const Object& obj);
//String operator+(const Object& obj, const char* str);

class StringHelper : public String
{
public:
	StringHelper(const String& s) : String(s) {}
	StringHelper(const char* p) : String(p) {}
	StringHelper(char c) : String(c) {}
	StringHelper(byte num) : String(num) {}
	StringHelper(int num) : String(num) {}
	StringHelper(uint num) : String(num) {}
	StringHelper(Int64 num) : String(num) {}
	StringHelper(ulong num) : String(num) {}
	StringHelper(float num) : String(num) {}
	StringHelper(double num) : String(num) {}
};

#endif
