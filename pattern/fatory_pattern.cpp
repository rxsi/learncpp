/*
创建型:
工厂模式
*/
#include <iostream>
#include <string>

class Shape
{
public:
    virtual void draw() = 0; // 要实现多态,那么需要需要把父类的函数定义为抽象方法
    // 1. 要么把这个方法定义为纯虚函数,即 virtual void draw() = 0;
    // 2. 要么完成这个方法的定义
    // {
    //     std::cout << "Shape::draw()" << std::endl;
    // }
};

class Rectangle: public Shape
{
public:
    void draw() override
    {
        std::cout << "Rectangle::draw()" << std::endl;
    }
};

class Square: public Shape
{
public:
    void draw() override
    {
        std::cout << "Square::draw()" << std::endl;
    }
};

class ShapeFactory
{
public:
    Shape* getShape(std::string shapeType)
    {
        if (shapeType == "Rectangle")
        {
            return new Rectangle();
        }
        else if (shapeType == "Square")
        {
            return new Square();
        }
        else
        {
            return nullptr;
        }
    }
};

int main()
{
    ShapeFactory factory;
    Shape* shape1 = factory.getShape("Rectangle");
    shape1->draw();
    Shape* shape2 = factory.getShape("Square");
    shape2->draw();
    delete shape1;
    delete shape2;
}