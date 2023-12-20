
#ifndef RINGBUF_H
#define RINGBUF_H



#include <iostream>

                       // По статусам можно отслеживать состояние очереди

enum class Deq_st {    // статус операции взятия элемента
    DEQ_OK,
    DEQ_EMPTY
};

enum class Enq_st {    // статус операции добавления элемента
    ENQ_OK,
    ENQ_FULL
};


template <typename T, int CAPACITY>
class RingBuffer {

public:

    template <typename T_COPY, int CAPACITY_COPY> friend class RingBuffer; // чтобы работал шаблонный конструктор

    RingBuffer() {}

    RingBuffer(const RingBuffer& src ) {
        std::cout<<"copy constructor"<<std::endl;             // проверка вызова конструктора копирования
        for(int i=0; i < CAPACITY; i++)  arr[i] = src.arr[i];
        pFront = src.pFront;
        pRear = src.pRear;
        enq = src.enq;
        deq = src.deq;
    }

    RingBuffer(RingBuffer&& src) {
        std::cout<<"move constructor"<<std::endl;             // проверка вызова конструктора перемещения
        for(int i=0; i < CAPACITY; i++) {
            arr[i] = src.arr[i];
            src.arr[i] = T();
        }
        pFront = src.pFront; src.pFront = pFront_default;
        pRear = src.pRear; src.pRear = pRear_default;
        enq = src.enq; src.enq = Enq_st::ENQ_OK;
        deq = src.deq; src.deq = Deq_st::DEQ_EMPTY;
    }

    template <typename T_COPY, int CAPACITY_COPY>
    RingBuffer(const RingBuffer<T_COPY, CAPACITY_COPY>& src) {
        std::cout<<"template copy constructor"<<std::endl;    // проверка вызова шаблонного конструктора копирования
        for(int i=0; i < CAPACITY; i++)  arr[i] = (src.arr[i]);
        pFront = src.pFront;
        pRear = src.pRear;
        enq = src.enq;
        deq = src.deq;
    }

    RingBuffer& operator= (const RingBuffer& eq) {
        std::cout<<"op= used"<<std::endl;                     // проверка использования оператора =
        for(int i=0; i < CAPACITY; i++)  arr[i] = eq.arr[i];
        pFront = eq.pFront;
        pRear = eq.pRear;
        enq = eq.enq;
        deq = eq.deq;
        return *this;
    }

    template <typename T_COPY, int CAPACITY_COPY>
    RingBuffer& operator= (const RingBuffer<T_COPY, CAPACITY_COPY>& eq) {
        std::cout<<"template op= used"<<std::endl;             // проверка использования шаблонного оператора =
        for(int i=0; i < CAPACITY; i++)  arr[i] = eq.arr[i];
        pFront = eq.pFront;
        pRear = eq.pRear;
        enq = eq.enq;
        deq = eq.deq;
        return *this;
    }
    
    ~RingBuffer() {}

    Enq_st enqueue(const T& elm) {

        if(enq == Enq_st::ENQ_OK) {
//            arr[pRear++] = elm;
new (&arr[pRear++]) T(elm);                 // placement new
            if(pRear == CAPACITY)               // если вышли за границы массива, возврат в начало
                pRear = 0;

            if(pRear == pFront) {               // "откатываем" указатель, если очередь заполнена
                if(pRear == 0)
                    pRear = CAPACITY-1;
                else
                    pRear--;
                enq = Enq_st::ENQ_FULL;
            }
        }
        else if(enq == Enq_st::ENQ_FULL) {
            if(pRear != CAPACITY-1 and pRear+1 != pFront)
                enq = Enq_st::ENQ_OK;
            else if(pRear == CAPACITY-1 and pFront != 0)
                enq = Enq_st::ENQ_OK;
        }

        return enq;
    }

    Deq_st dequeue(T& elm) {

        if(deq == Deq_st::DEQ_EMPTY) {                 // если очередь была пуста, а затем были добавлены элементы
            if(pFront == CAPACITY-1 and pRear != 0) {     // двигаем на первый добавленный
                pFront = 0;
                deq = Deq_st::DEQ_OK;
            }
            else if(pFront+1 != pRear) {
                pFront++;
                deq = Deq_st::DEQ_OK;
            }
        }

        if(deq == Deq_st::DEQ_OK) {
            elm = arr[pFront];
//            arr[pFront] = T();
            new (&arr[pFront]) T();             // placement new, очистка элемента, вызов его конструктора
            pFront++;

            if(pFront == CAPACITY)              // если вышли за границы массива, возврат в начало
                pFront = 0;

            if(pFront == pRear) {               // проверка опустошения очереди
                if(pFront == CAPACITY)
                    pFront = CAPACITY-1;
                else
                    pFront--;                       // "откатываем" pFront, если очередь пуста
                deq = Deq_st::DEQ_EMPTY;
            }
        }

        return deq;
    }

    int get_cap() const {
        return CAPACITY;   
    }

    T get_elm(int idx) const {
        return arr[idx];   
    }

    int get_pRear() {
        return pRear;
    }
    
    int get_pFront() {
        return pFront;
    }

    Enq_st getEnq() {      // функция проверки состояния очереди
        return enq;
    }

    Deq_st getDeq() {      // функция проверки состояния очереди
        return deq;
    }

//    T& operator= (const T&);    // оператор = класса (типа) Т
//    T:T();                      // делегирование конструктора класса T

private:

    const int pRear_default{1}, pFront_default{0};

    T arr[CAPACITY]{};
    int pRear{pRear_default};
    int pFront{pFront_default};
    enum Enq_st enq{Enq_st::ENQ_OK};
    enum Deq_st deq{Deq_st::DEQ_EMPTY};

};



#endif //RINGBUF_H
