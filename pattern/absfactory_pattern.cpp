/*
创建型:
抽象工厂模式
以一个总的工厂类包含所有的工厂类
*/
#include <iostream>
#include <string>

class Shape
{
public:
    virtual void draw() = 0;
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

class Color
{
public:
    virtual void fill() = 0;
};

class Red: public Color
{
public:
    void fill()
    {
        std::cout << "Red::fill()" << std::endl;
    }
};

class Green: public Color
{
public:
    void fill()
    {
        std::cout << "Green::fill()" << std::endl;
    }
};

class AbstractFactory
{
public:
    // Color/Shape都是抽象类,因此只能返回指针
    virtual Color* getColor(std::string color) = 0;
    virtual Shape* getShape(std::string shape) = 0;
};

class ShapeFactory: public AbstractFactory
{
public:
    Color* getColor(std::string color) override
    {
        return nullptr;
    }

    Shape* getShape(std::string shape) override
    {
        if (shape == "Rectangle")
        {
            return new Rectangle();
        }
        else if (shape == "Square")
        {
            return new Square();
        }
        else
        {
            return nullptr;
        }
    }
};

class ColorFactory: public AbstractFactory
{
public:
    Color* getColor(std::string color) override
    {
        if (color == "Red")
        {
            return new Red();
        }
        else if (color == "Green")
        {
            return new Green();
        }
        else
        {
            return nullptr;
        }
    }

    Shape* getShape(std::string shape) override
    {
        return nullptr;
    }
};

class FactoryProducer
{
public:
    static AbstractFactory* getFactory(std::string factory) // 可以设计为静态方法
    {
        if (factory == "Shape")
        {
            return new ShapeFactory();
        }
        else if (factory == "Color")
        {
            return new ColorFactory();
        }
        else 
        {
            return nullptr;
        }
    }
};

int main()
{
    AbstractFactory* fatory1 = FactoryProducer::getFactory("Shape");
    Shape* shape1 = fatory1->getShape("Rectangle");
    shape1->draw();
    Shape* shape2 = fatory1->getShape("Square");
    shape2->draw();

    AbstractFactory* fatory2 = FactoryProducer::getFactory("Color");
    Color* color1 = fatory2->getColor("Red");
    color1->fill();
    Color* color2 = fatory2->getColor("Green");
    color2->fill();

    delete shape1;
    delete shape2;
    delete color1;
    delete color2;
    delete fatory1;
    delete fatory2;
}