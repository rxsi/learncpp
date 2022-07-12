#include <iostream>
#include <vector>

using namespace std;

/*
九宫格AOI

1. 将一个地图切分为多个格子，每个格子占用多个像素，因此实际上不能做到那么精细
2. 客户端发送过来的坐标代表的是像素坐标，因此需要转换为九宫格坐标，这意味着在一个格子内发送过来的多个像素坐标实际上是同一个格子内
3. 以手机屏幕为例，一个屏幕不能完整的显示整个九宫格的内容，也就是说一个格子是很大的，所以当一个玩家在一个格子里面运动时，虽然x、y会变化，但是实际视野不会变化的（因为所占的九宫格没有变化）
2. 不同的视野
3. 对于场景中一个很大的物体，
*/

class gridAOI
{
public:
    gridAOI();
    ~gridAOI();
    
    gridAOI(const gridAOI&) = delete;
    gridAOI(const gridAOI&&) = delete;
    gridAOI& operator=(const gridAOI&) = delete;
    gridAOI& operator=(const gridAOI&&) = delete;

public:

    int entity_enter(int, int, int, int); // 进入
    int entity_leave(int); // 离开当前九宫格
    int entity_update(int, int, int); // 更新位置


private:
    struct entity // 为场景中的每一个玩家/怪物/NPC创建一个映射对象
    {
        int entityID;
        int x;
        int y;
        vector<entity*> interest_me;
    }
};