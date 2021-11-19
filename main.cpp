#include <iostream>


class Grid; class Memory_cell;


class Memory_cell{ // memory cell хранит указатели на объект float(число) или на объект Grid(сетка)
public:
    int type =0 ; // 0 соответствует числу, 1 - сетке
    union {
        float *p;
        Grid *g;
    };
    Memory_cell(): type(0) {p = new float(0);}
    float get_value() const;



    Memory_cell(float f): type(0){
        p = new float(f);
    }

    ~Memory_cell();

    Memory_cell(size_t x_size, size_t y_size);

    Memory_cell& operator=(float);

};

class Grid{
private:
    Memory_cell *memory;
    size_t x_size, y_size;
public:
    Grid(size_t x_size, size_t y_size): x_size(x_size), y_size(y_size), memory(new Memory_cell[x_size*y_size]){ }
    ~Grid(){delete[] memory;}

    float get_value() const;

    size_t get_xsize() const {return x_size;}
    size_t get_ysize() const {return y_size;}

    float operator()(size_t x_idx, size_t y_idx) const;

    Grid& operator=(float);

    Memory_cell& operator()(size_t x_idx, size_t y_idx);

    friend std::ostream& operator<<(std::ostream& os, Grid const& grid){
        for (int j=0; j < grid.y_size; j++){
            for (int i=0; i<grid.x_size-1;i++){
                os<< grid(i, j) << " ";
            }
            os<<" " << grid(grid.x_size-1, j)<<std::endl;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Grid & grid){
        for (int j=0; j<grid.y_size;j++){
            for (int i = 0; i < grid.x_size; ++i) {
                float f;
                is >> f;
                (grid.memory[j*grid.x_size+i]).operator=(f);
            }
        }
        return  is;
    }


    Grid& make_subgrid(size_t x_idx, size_t y_idx, size_t x_sub_size, size_t y_sub_size);

    Grid& collapse_subgrid(
            size_t x_idx, size_t y_idx,
            size_t x_sub_size, size_t y_sub_size);

    Grid& get_subgrid(size_t x_idx, size_t y_idx);


    bool is_subrid(size_t x_idx, size_t y_idx) const;

};

float Memory_cell::get_value() const {
if (type ==0) { return *p;}
else if (type==1) {return g->get_value();}
else return -1;
}

float Grid::get_value() const {
    float sum =0;
    for (int i=0; i< x_size*y_size;i++){
        sum += memory[i].get_value();
    }
    return sum/(x_size*y_size);
}

float Grid::operator()(size_t x_idx, size_t y_idx) const {
    return memory[x_idx+x_size*y_idx].get_value();
}

Memory_cell& Grid::operator()(size_t x_idx, size_t y_idx) {
    return memory[x_idx+x_size*y_idx];
}

Memory_cell::Memory_cell(size_t x_size, size_t y_size) :type(1) {
    g = new Grid(x_size, y_size);
}

Memory_cell::~Memory_cell() {
        if (type==0) {delete p;}
        else if (type ==1) {delete g;}
        delete this;
    }

Memory_cell& Memory_cell::operator=(float f) {
    if (type==0){*p =f;}
    else if (type==1) {g->operator=(f);}
    return *this;
}

Grid& Grid::operator=(float f) {
    for (int i=0; i<x_size*y_size; i++){
        memory[i].operator=(f);
    }

    return *this;
}


Grid& Grid::make_subgrid(size_t x_idx, size_t y_idx, size_t x_sub_size, size_t y_sub_size){
    auto tmp = new Grid(x_sub_size, y_sub_size);
    *tmp= this->operator()(x_idx, y_idx).get_value(); //get_value работает как для числа так и для сетки(сред. арифм.)
    memory[x_idx+y_idx*x_size].type=1; //превращаем memory_cell в сетку
    memory[x_idx+y_idx*x_size].g = tmp; // на старую сетку/число больше ничего не указывет - деструктор
    return *this;
}

Grid& Grid::collapse_subgrid(size_t x_idx, size_t y_idx, size_t x_sub_size, size_t y_sub_size) {
    auto tmp = new float;
    *tmp = this->operator()(x_idx, y_idx).get_value();
    memory[x_idx+y_idx*x_size].type=0; //превращаем memory_cell в число
    memory[x_idx+y_idx*x_size].p = tmp; // на старую сетку/число больше ничего не указывет - деструктор
    return *this;
}

Grid& Grid::get_subgrid(size_t x_idx, size_t y_idx) {
    return *((this->operator()(x_idx, y_idx)).g);
}

bool Grid::is_subrid(size_t x_idx, size_t y_idx) const {
    return (memory[x_idx+y_idx*x_size].type==1);
}

int main(){
    auto b = new Grid (2, 2);
    std::cin >> *b;
    (*b)(0, 1)=5;
    std::cout << *b << b->is_subrid(1, 1)<<'\n';
    b->make_subgrid(1, 1, 2, 2);
    std::cout << *(b->operator()(1, 1).g) << (*b).is_subrid(1, 1)<<'\n';
    (*b)(1, 1)=100;
    std::cout << *(b->operator()(1, 1).g) << (*b).is_subrid(1, 1)<<'\n';

    return 0;
}

