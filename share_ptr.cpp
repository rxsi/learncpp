/*
智能指针的一些探索
*/

// 1. std::auto_ptr 已弃用, 因为他的复制语句会使旧的对象失去所持有的堆内存,造成使用上的异常

// 2. std::unique_ptr, 底层实现中将拷贝构造/拷贝赋值都定义为delete.
// 但是可以使用移动构造和移动赋值语句

// 3. std::shared_ptr, 对于引用计数的增加和减少是线程安全的(可以看到有用过thread_safexxx方法保证)
// 底层使用share_count类记录当前对象的引用计数
/*
      __shared_count&
      operator=(const __shared_count& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi; // 将传入的对象的引用计数类(share_count) 转换为_Sp_counted_base类
	if (__tmp != _M_pi) // 如果两个不一致,也就是不是把自己的对象移动赋值给自己
	  {
	    if (__tmp != 0) // 如果传入对象的引用计数不为0,则先增加他的引用计数
	      __tmp->_M_add_ref_copy();
	    if (_M_pi != 0) // 如果本对象的引用计数不为0,则递减引用计数
	      _M_pi->_M_release(); // 线程安全的递减,递减后如果为0了,则释放对象,即会调用对象的析构函数
	    _M_pi = __tmp;
	  }
	return *this;
      }
*/
// 引用计数的实现本质:
// 当我们通过shared_ptr<T> 定义T对象时,会定义一个shared_ptr对象(父类对象是__shared_ptr),主要保存一个T对象指针和一个shared_count对象
// shared_count对象负责记录引用数量,内部会生成一个指向_Sp_counted_ptr对象的指针
// 当我们把Shared_ptr对象拷贝或者赋值给别的对象时,会复制本shared_ptr对象的T对象指针(内置类型,不会有多个对象) 和 复制shared_count对象,
// shared_count对象的复制会把保存的_Sp_counted_ptr指针复制给别的对象.然后进行新旧对象引用计数的增减
// 当shared_ptr对象释放时,会调用父类的析构函数,然后该析构函数会调用shared_count对象的递减函数,当递减为0,则真正的析构掉T对象(通过T指针)

// 因此本质上只有一个T对象和_Sp_counted_ptr对象,且定义在堆内存,需要手动进行释放
// 而Shared_ptr, shared_count定义在栈内存,因此当栈退出后,会进行析构
// shared_ptr对象析构时会进行shared_count对象的析构,shared_count对象的析构则会调用_Sp_counted_ptr指针进行引用计数的递减,如果为0则释放T对象

// 4. std::weak_ptr, 不会增加引用计数
// 本质上和shared_ptr 类似,也是保存了T对象指针和_Sp_counted_ptr对象指针
// 不过区别的是当增加/减少引用修改的是_Sp_counted_ptr对象的weak计数

#include<iostream>
#include<memory>
using namespace std;

unique_ptr<int> make(int i){
    return make_unique<int>(i);
}

int main(){
    // 1. auto_ptr
    // std::auto_ptr<int> pt1(new int(8));
    // std::auto_ptr<int> pt2(pt1);
    // if(pt1.get() == NULL){ 
    //     cout << "pt1 is NULL" << endl;
    // }

    // // 2. unique_ptr
    // auto pt1 = make_unique<int>(123); 
    // unique_ptr<int> pt2(move(pt1)); // 移动构造
    // auto pt3 = make_unique<int>(435);
    // pt3 = move(pt2); // 移动赋值
    // // 因此也可以通过一个函数返回unique_ptr,因为这个返回值是个右值.相当于调用移动构造
    // auto pt4 = make(1);

    // // unique_ptr, shared_ptr, weak_ptr都可以持有一组堆对象
    // auto pt5 = make_unique<int []>(10);
    // for (int i = 0; i < 10; i++){
    //     pt5[i] = i;
    // }
}