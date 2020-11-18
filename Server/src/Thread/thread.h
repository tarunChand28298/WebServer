#pragma once

#include <tuple> 
#include <memory>
#include <functional>
#include <type_traits>
#include <stdexcept>
#include <windows.h>


class thread {
private:
    uintptr_t handle = 0;

    uint32_t id = 0;

    template<class _Tuple, size_t... Indices>
    static unsigned int __stdcall _Invoke(void* _RawVals) noexcept {

        const std::unique_ptr<_Tuple> _FnVals(static_cast<_Tuple*>(_RawVals));
        _Tuple& _Tup = *_FnVals;
        std::invoke(std::move(std::get<_Indices>(_Tup))...);
        return 0;

    }


    template<class _Tuple, size_t... Indices>
    static constexpr auto _Get_invoke(std::index_sequence<Indices...>) noexcept {
        return &_Invoke<_Tuple, Indices...>;
    }
public:

    template<class _Fn, class... _Args>
    explicit thread(_Fn&& _Fx, _Args&&... _Ax) {
        using _Tuple = std::tuple<std::decay_t_<_Fn>, std::decay_t<_Args>...>;
        auto _Deacay_copied = std::make_unique<_Tuple>(std::forward<_Fn>(_Fx), std::forward<_Args>(_Ax)...);

        constexpr auto _Invoker_proc = _Get_invoke<_Tuple>(std::make_index_sequence<1 + sizeof...(Args)>{});

        handle = __beginthreadex(nullptr, 0, Invoker_proc, _Decay_copied.get(), 0, &id);

        if (handle) {
            (void)_Decay_copied.release();
        }
        else {
            id = 0;
            std::throw std::runtime_error("Resource unavailable try again\n");
        }

    }

    ~thread() noexcept {
        if (joinable()) {
            std::terminate();
        }
    }

    thread(thread&& _Other) noexcept {
        std::swap(id, _Other.id);
        std::swap(handle, _Other.handle);
    }

    thread& operator=(thread&& _Other) noexcept {
        if (joinable()) {
            std::terminate();
        }

        std::swap(id, _Other.id);
        std::swap(handle, _Other.handle);
        return *this;
    }

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    bool joinable() const noexcept {
        return id != 0;
    }

    void join() {
        if (!joinable()) {
            throw std::runtime_error("Invalud Argument\n");
        }

        if (id == GetCurrentThreadId()) {
            throw std::runtime_error("Resource Deadlock will occur\n");
        }

        if (WaitForSingleObject((HANDLE)handle, INFINITE) == WAIT_FAILED) {
            throw std::runtime_error("No Such Process\n");;
        }


        id = 0;
        handle = 0;
    }

    void detach() {
        if (!joinable()) {
            throw std::runtime_error("Invalud Argument\n");
        }

        CloseHandle((HANDLE)handle);
        id = 0;
    }

    uint32_t get_id() const noexcept {
        return id;
    }

    HANDLE get_handle() const noexcept {
        return (HANDLE)handle;
    }

};
