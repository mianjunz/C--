/* 修饰变量 */
const int a = 100;  // a 是常量，值不能被修改
// a = 200;  // 错误：a 是常量，不能被修改
/*
说明：const 修饰的变量在初始化后，其值不能被修改。
*/

/* 修饰函数参数 */
void func(const int x)
{
	// x = 200;  // 错误：x 是常量，不能被修改
}
/*
说明：const 修饰函数参数，表示函数体内不能修改该参数的值。
这通常用于防止函数意外修改参数值。
*/

/* 修饰函数返回值 */
const char *getstr(void)
{
	return "Hello, World!";  // 返回的是字符串常量
}
/*
说明：const 修饰函数返回值，表示返回值不能被修改。
这通常用于返回字符串常量或其他不可修改的对象。
*/

/* 修饰指针 */
void pointer_const_demo(void)
{
	/* 1. 常量指针：指针指向的内容不能修改，但指针的地址可以修改。 */
	{
		const int *pa;            // 等同于 int const *pa;
		const int b = 100;
		const int c = 200;
		pa = &b;
		// *pa = 200;  // 错误：pa 指向的内容不能修改
		pa = &c;      // 正确：pa 的地址可以修改
	}
	/*
	说明：int const *a 表示 a 指向的内容是常量，不能修改 *a。
	*/

	/* 2. 指针常量：指针地址不能修改，但指针指向的内容可以修改。 */
	{
		int b = 100;
		int c = 200;
		int *const pa = &b;
		*pa = 200;    // 正确：pa 指向的内容可以修改
		// pa = &c;    // 错误：pa 的地址不能修改
		(void)c;
	}
	/*
	说明：int *const a 表示 a 的地址是常量，不能修改 a 的地址。
	*/

	/* 3. 常量指针常量：指针地址和指向内容都不能修改。 */
	{
		const int b = 100;
		const int c = 200;
		const int *const pa = &b;
		// *pa = 200;  // 错误：pa 指向的内容不能修改
		// pa = &c;    // 错误：pa 的地址不能修改
		(void)c;
	}
	/*
	说明：const int *const a 表示 a 的地址和指向的内容都是常量，都不能被修改。
	*/
}
