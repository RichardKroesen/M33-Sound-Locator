/* 
 * File:   circular_buffer.hpp
 * Author: r.kroesen
 *
 * Created on 9 August 2023
 */

#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

#include <cstdint>
#include "FreeRTOS.h"
#include "semphr.h"
#include "portmacro.h"

template <typename DataType, size_t BUFFER_SIZE>
class CircularBuffer {
public:
    CircularBuffer() {
        static_assert(BUFFER_SIZE != 0, "Error, circular buffer cannot have size of 0.");
        bufferMutex = xSemaphoreCreateMutex();
        reset();
    }

    ~CircularBuffer() {
        vSemaphoreDelete(bufferMutex);
    }

    void reset() {
        taskENTER_CRITICAL();
        bufferHead = 0;
        bufferTail = 0;
        currentSize = 0;
        runningSum = 0;
        taskEXIT_CRITICAL();
    }

    const bool isEmpty() {
        taskENTER_CRITICAL();
        const bool result = (currentSize == 0);
        taskEXIT_CRITICAL();
        return result;
    }

    const bool isFull() {
        taskENTER_CRITICAL();
        const bool result = (currentSize == BUFFER_SIZE);
        taskEXIT_CRITICAL();
        return result;
    }

    const size_t size() {
        taskENTER_CRITICAL();
        const size_t result = currentSize;
        taskEXIT_CRITICAL();
        return result;
    }

    void push(const DataType& value) {
        xSemaphoreTake(bufferMutex, portMAX_DELAY);
        push_internal(value);
        xSemaphoreGive(bufferMutex);
    }

    void pushFromISR(const DataType& value) {
        taskENTER_CRITICAL();
        push_internal(value);
        taskEXIT_CRITICAL();
    }

    const DataType pop() {
        xSemaphoreTake(bufferMutex, portMAX_DELAY);
        const DataType value = pop_internal();
        xSemaphoreGive(bufferMutex);
        return value;
    }

    const DataType popFromISR() {
        taskENTER_CRITICAL();
        const DataType value = pop_internal();
        taskEXIT_CRITICAL();
        return value;
    }

    DataType get(size_t index) {
        xSemaphoreTake(bufferMutex, portMAX_DELAY);
        if (index >= currentSize) {
            xSemaphoreGive(bufferMutex);
            return {}; // Handle invalid index
        }

        size_t bufferIndex = (bufferTail + index) % BUFFER_SIZE;
        DataType value = bufferArray[bufferIndex];
        xSemaphoreGive(bufferMutex);
        return value;
    }

    // DataType getAverage() {
    //     xSemaphoreTake(bufferMutex, portMAX_DELAY);
    //     if (currentSize == 0) {
    //         xSemaphoreGive(bufferMutex);
    //         return {}; // Handle empty case
    //     }

    //     DataType average = static_cast<DataType>(runningSum / currentSize);
    //     xSemaphoreGive(bufferMutex);
    //     return average;
    // }

private:
    DataType bufferArray[BUFFER_SIZE];
    size_t bufferHead = 0;
    size_t bufferTail = 0;
    size_t currentSize = 0;
    uint64_t runningSum = 0;
    SemaphoreHandle_t bufferMutex;

    // Advance head pointer
    void advanceHead() {
        bufferHead = (bufferHead + 1) % BUFFER_SIZE;
    }

    // Advance tail pointer
    void advanceTail() {
        bufferTail = (bufferTail + 1) % BUFFER_SIZE;
    }

    void push_internal(const DataType& value) {
        if (currentSize == BUFFER_SIZE) {
            // Buffer is full
            runningSum -= bufferArray[bufferTail];
            advanceTail();
        } else {
            currentSize++;
        }

        bufferArray[bufferHead] = value;
        runningSum += value;
        advanceHead();
    }

    const DataType pop_internal() {
        if (currentSize == 0) {
            return {}; // Handle empty buffer
        }

        const DataType value = bufferArray[bufferTail];
        runningSum -= value;
        advanceTail();
        currentSize--;
        return value;
    }
};

#endif // CIRCULAR_BUFFER_HPP