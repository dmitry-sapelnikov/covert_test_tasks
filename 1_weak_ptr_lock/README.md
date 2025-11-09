## Вопрос:
Как бы вы реализовали работу со счётчиком ссылок в методе `std::weak_ptr::lock()`?
Реализация приветствуется, но можно отразить словами основные аспекты.

## Ответ:
Я бы реализовал метод следующим образом:
```cpp
template <typename T>
[[nodiscard]] WeakPtr<T>::lock() const noexcept
{
    SharedPtr<T> result;
    result.construct_from_weak(*this);
    return result;
}
```

Метод `SharedPtr::construct_from_weak`:
```cpp
template <typename T>
void SharedPtr<T>::construct_from_weak(const WeakPtr<T> &weak_ptr) noexcept
{
    if (weak_ptr.m_control_block != nullptr &&
        weak_ptr.m_control_block->increment_strong_ref_not_zero())
    {
        m_control_block = weak_ptr.m_control_block;
        // Here it should also be assignment of the resource pointer,
		// but we omit it for simplicity
    }
}
```

Самая интересная часть - увеличение strong reference counter-а.
Если потокобезопасность **не** требуется, то реализация тривиальна.
Можно использовать просто `long int` или `long unsigned int` для счетчика
и реализовать метод следующим образом:
```cpp
template <typename T>
bool SharedPtr<T>::increment_strong_ref_not_zero() noexcept
{
    if (m_ref_counter != 0)
    {
        ++m_ref_counter;
        return true;
    }
    return false;
}
```

Для потокобезопасной версии рационально использовать `std::atomic<long int>` или `std::atomic<long unsigned int>`:
```cpp
template <typename T>
bool SharedPtr<T>::increment_strong_ref_not_zero() noexcept
{
    long int count = m_ref_counter.load(std::memory_order_relaxed);
    while (count != 0)
    {
        if (m_ref_counter.compare_exchange_weak(
            count,
            count + 1,
            std::memory_order_relaxed,
            std::memory_order_relaxed))
        {
            return true;
        }
    }
    return false;
}
```

Мы можем позволить себе использовать relaxed memory ordering, т.к. случаи, когда
нам могла бы понадобится более строгая синхронизация помимо атормарности, в данном методе отсутстуют:
у нас нет других операций с памятью помимо нашего счетчика.

Цикл необходим для случая, когда инкремент происходит в период времени между первым чтением и занулением
счетчика из другого потока.

Реализация фактически - калька с настоящей реализации из MSVC с заменой `volatile` счетчика и вызовов atomic instrinsics на `std::atomic`.

Прототип реализации и нескольких базовых тестов находится в [`weak_ptr_lock.cpp`](weak_ptr_lock.cpp)
