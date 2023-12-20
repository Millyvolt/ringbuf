/**************************************************************

    Threadsafing template queue. Thereadsafe in case 2 threads, when one thread writing to queue and another reading from it.

    Running in Qt 5.14.1, C++17.

**************************************************************/



#include <iostream>
#include <thread>
#include <utility>
#include <memory>

#include "ringbuf.h"


using std::cout;
using std::endl;


class Fraction {
    int numerator;
    int denominator;
public:
    Fraction(int num = 0, int den = 1): numerator(num), denominator(den) {};
    Fraction(const Fraction& src) {
        numerator = src.numerator;
        denominator = src.denominator;
    }
    ~Fraction() {};

    void setNum(int num) {
        numerator = num;
    }

    void setDen(int den) {
        denominator = den;
    }

    double getFraction () const {
        return static_cast<double>(numerator) / static_cast<double>(denominator);
    }

    friend std::ostream& operator<<(std::ostream& out, const Fraction& f)
    {
       return out << f.getFraction();
    }
};


int main() {

    int elm_put_size(12), elm_put[elm_put_size];
    const int queue_size(10);


    RingBuffer<int, queue_size> buf1;

       // создадим 1ю очередь и запишем элементы (кол-во элементов больше очереди)
    cout<<endl<<"Create and fill 1st queue:"<<endl;
    for(int i=0; i < elm_put_size; i++) {
        elm_put[i] = i+1;
        buf1.enqueue(elm_put[i]);
    }
    cout<<"buf1: "; for(int i=0; i < queue_size; i++)  cout<<buf1.get_elm(i)<<" "; cout<<endl;
    cout<<"buf1.cap: "<<buf1.get_cap()<<endl;
    cout<<"buf1.pFront: "<<buf1.get_pFront()<<endl;
    cout<<"buf1.pRear: "<<buf1.get_pRear()<<endl;

        // проверим конструктор копирования
    cout<<endl<<"Copy constructor test:"<<endl;

    RingBuffer<int, queue_size> buf2(buf1);

    cout<<"buf2: "; for(int i=0; i < queue_size; i++)  cout<<buf2.get_elm(i)<<" "; cout<<endl;
    cout<<"buf2.cap: "<<buf2.get_cap()<<endl;
    cout<<"buf2.pFront: "<<buf2.get_pFront()<<endl;
    cout<<"buf2.pRear: "<<buf2.get_pRear()<<endl;

        // заберем несколько элементов из очереди buf2
    const int elm_get_size = 3;
    int elm_get[elm_get_size];
    cout<<endl<<"Take "<<elm_get_size<<" elements from buf2:"<<endl;
    for(int i=0; i < elm_get_size; i++)  buf2.dequeue(elm_get[i]);
    cout<<"elements taken from buf2: ";for(int i=0; i < elm_get_size; i++) cout<<elm_get[i]<<" "; cout<<endl;
    cout<<"buf2: "; for(int i=0; i < queue_size; i++)  cout<<buf2.get_elm(i)<<" "; cout<<endl;
    cout<<"buf2.pFront: "<<buf2.get_pFront()<<endl;
    cout<<"buf2.pRear: "<<buf2.get_pRear()<<endl;

        // проверим оператор =
    cout<<endl<<"Operator = testing:"<<endl;

    RingBuffer<double, queue_size> buf3;

    buf3 = buf2;

    cout<<"buf3: "; for(int i=0; i < queue_size; i++)  cout<<buf3.get_elm(i)<<" "; cout<<endl;


        // запустим 2 потока
    cout<<endl<<"Threads start:"<<endl;

    cout<<"buf1: "; for(int i=0; i < queue_size; i++)  cout<<buf1.get_elm(i)<<" "; cout<<endl;
    cout<<"buf1.pFront: "<<buf1.get_pFront()<<endl;
    cout<<"buf1.pRear: "<<buf1.get_pRear()<<endl;

    elm_put[0] = 45;


    std::thread thr_rd(&RingBuffer<int, queue_size>::dequeue, &buf1, std::ref(elm_get[0]));
    std::thread thr_wr(&RingBuffer<int, queue_size>::enqueue, &buf1, std::ref(elm_put[0]));

    thr_rd.join();
    thr_wr.join();


    cout<<"elm_get: "<<elm_get[0]<<endl;
    cout<<"buf1: "; for(int i=0; i < queue_size; i++)  cout<<buf1.get_elm(i)<<" "; cout<<endl;
    cout<<"buf1.pFront: "<<buf1.get_pFront()<<endl;
    cout<<"buf1.pRear: "<<buf1.get_pRear()<<endl;


        // создадим очередь c элементами класса Fraction
    cout<<endl<<"Create and fill class Fraction queue:"<<endl;

    Fraction frct_arr[elm_put_size];
    RingBuffer<Fraction, queue_size> buf_fr;

    for(int i=0; i < elm_put_size; i++) {
        frct_arr[i].setNum(i);
        frct_arr[i].setDen(i+1);
        buf_fr.enqueue(frct_arr[i]);
    }

    cout<<"buf_fr: "; for(int i=0; i < queue_size; i++)  cout<<buf_fr.get_elm(i)<<" "; cout<<endl;

    RingBuffer<Fraction, queue_size> buf_fr2(buf_fr);
    cout<<"buf_fr2: "; for(int i=0; i < queue_size; i++)  cout<<buf_fr2.get_elm(i)<<" "; cout<<endl;
    RingBuffer<Fraction, queue_size> buf_fr3;
    buf_fr3 = buf_fr2;
    cout<<"buf_fr3: "; for(int i=0; i < queue_size; i++)  cout<<buf_fr3.get_elm(i)<<" "; cout<<endl;
    cout<<"buf_fr3.pFront: "<<buf_fr3.get_pFront()<<endl;
    cout<<"buf_fr3.pRear: "<<buf_fr3.get_pRear()<<endl;

    // заберем несколько элементов из очереди buf_fr3
    Fraction elm_get_fr[elm_get_size]{};
    cout<<endl<<"Take "<<elm_get_size<<" elements from buf_fr3:"<<endl;
    for(int i=0; i < elm_get_size; i++)  buf_fr3.dequeue(elm_get_fr[i]);
    cout<<"elements taken from buf_fr3: ";for(int i=0; i < elm_get_size; i++) cout<<elm_get_fr[i]<<" "; cout<<endl;
    cout<<"buf_fr3: "; for(int i=0; i < queue_size; i++)  cout<<buf_fr3.get_elm(i)<<" "; cout<<endl;
    cout<<"buf_fr3.pFront: "<<buf_fr3.get_pFront()<<endl;
    cout<<"buf_fr3.pRear: "<<buf_fr3.get_pRear()<<endl<<endl;

    RingBuffer<Fraction, queue_size> buf_fr4(std::move(buf_fr3));
    cout<<"buf_fr4: "; for(int i=0; i < queue_size; i++)  cout<<buf_fr4.get_elm(i)<<" "; cout<<endl;
    cout<<"buf_fr3: "; for(int i=0; i < queue_size; i++)  cout<<buf_fr3.get_elm(i)<<" "; cout<<endl;




    RingBuffer<std::shared_ptr<Fraction>, queue_size> buf_smart_ptr;
    cout<<"buf_smart_ptr data: ";
    std::shared_ptr<Fraction> ptr1(new Fraction(3,4));
    std::shared_ptr<Fraction> ptr2(new Fraction(5,6));
    std::shared_ptr<Fraction> ptr3(new Fraction(7,8));
    buf_smart_ptr.enqueue(ptr1);
    buf_smart_ptr.enqueue(ptr2);
    buf_smart_ptr.enqueue(ptr3);
    cout<<*(buf_smart_ptr.get_elm(1))<<" ";
    cout<<*(buf_smart_ptr.get_elm(2))<<" ";
    cout<<*(buf_smart_ptr.get_elm(3))<<" ";
    cout<<endl;

    RingBuffer<std::shared_ptr<Fraction>, queue_size> buf_smart_ptr1(std::move(buf_smart_ptr));
    cout<<"buf_smart_ptr1 data: ";
    cout<<*(buf_smart_ptr1.get_elm(1))<<" ";
    cout<<*(buf_smart_ptr1.get_elm(2))<<" ";
    cout<<*(buf_smart_ptr1.get_elm(3))<<" ";
    cout<<endl;
    cout<<"buf_smart_ptr now is empty "<<endl;


    RingBuffer<std::shared_ptr<Fraction>, queue_size> buf_smart_ptr2;
    buf_smart_ptr2 = buf_smart_ptr1;
    cout<<"buf_smart_ptr2 data: ";
    cout<<*(buf_smart_ptr2.get_elm(1))<<" ";
    cout<<*(buf_smart_ptr2.get_elm(2))<<" ";
    cout<<*(buf_smart_ptr2.get_elm(3))<<" ";
    cout<<endl;

    // заберем несколько элементов из очереди buf_smart_ptr2
    cout<<endl<<"Take 2 elements from buf_smart_ptr2: ";
    buf_smart_ptr2.dequeue(ptr1);
    buf_smart_ptr2.dequeue(ptr2);
    cout<<*ptr1<<" "<<*ptr2<<endl;

}


    // Тестирование класса очереди:
/*
Create and fill 1st queue:
buf1: 0 1 2 3 4 5 6 7 8 9
buf1.cap: 10
buf1.pFront: 0
buf1.pRear: 9

Copy constructor test:
copy constructor
buf2: 0 1 2 3 4 5 6 7 8 9
buf2.cap: 10
buf2.pFront: 0
buf2.pRear: 9

Take 3 elements from buf2:
elements taken from buf2: 1 2 3
buf2: 0 0 0 0 4 5 6 7 8 9
buf2.pFront: 4
buf2.pRear: 9

Operator = testing:
template op= used
buf3: 0 0 0 0 4 5 6 7 8 9

Threads start:
buf1: 0 1 2 3 4 5 6 7 8 9
buf1.pFront: 0
buf1.pRear: 9
elm_get: 1
buf1: 0 0 2 3 4 5 6 7 8 9
buf1.pFront: 2
buf1.pRear: 9

Create and fill class Fraction queue:
buf_fr: 0 0 0.5 0.666667 0.75 0.8 0.833333 0.857143 0.875 0.888889
copy constructor
buf_fr2: 0 0 0.5 0.666667 0.75 0.8 0.833333 0.857143 0.875 0.888889
op= used
buf_fr3: 0 0 0.5 0.666667 0.75 0.8 0.833333 0.857143 0.875 0.888889
buf_fr3.pFront: 0
buf_fr3.pRear: 9

Take 3 elements from buf_fr3:
elements taken from buf_fr3: 0 0.5 0.666667
buf_fr3: 0 0 0 0 0.75 0.8 0.833333 0.857143 0.875 0.888889
buf_fr3.pFront: 4
buf_fr3.pRear: 9

move constructor
buf_fr4: 0 0 0 0 0.75 0.8 0.833333 0.857143 0.875 0.888889
buf_fr3: 0 0 0 0 0 0 0 0 0 0
buf_smart_ptr data: 0.75 0.833333 0.875
copy constructor
buf_smart_ptr1 data: 0.75 0.833333 0.875
op= used
buf_smart_ptr2 data: 0.75 0.833333 0.875

Take 2 elements from buf_smart_ptr2:0.75 0.833333
*/





