#pragma once

#include <Windows.h>
#include <bee/subprocess/common.h>
#include <bee/utility/zstring_view.h>

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace bee::subprocess {
    namespace ignore_case {
        template <class T>
        struct less;
        template <>
        struct less<wchar_t> {
            bool operator()(const wchar_t& lft, const wchar_t& rht) const noexcept {
                return (towlower(static_cast<wint_t>(lft)) < towlower(static_cast<wint_t>(rht)));
            }
        };
        template <>
        struct less<std::wstring> {
            bool operator()(const std::wstring& lft, const std::wstring& rht) const {
                return std::lexicographical_compare(lft.begin(), lft.end(), rht.begin(), rht.end(), less<wchar_t>());
            }
        };
    }

    enum class console {
        eInherit,
        eDisable,
        eNew,
        eDetached,
        eHide,
    };
    class envbuilder {
    public:
        void set(const std::wstring& key, const std::wstring& value);
        void del(const std::wstring& key);
        environment release();

    private:
        using less = ignore_case::less<std::wstring>;
        std::map<std::wstring, std::wstring, less> set_env_;
        std::set<std::wstring, less> del_env_;
    };

    using process_id = uint32_t;
    using process_handle = void*;

    class spawn;
    class process {
    public:
        process() noexcept;
        process(process&& o) noexcept;
        process(spawn& spawn) noexcept;
        ~process() noexcept;
        std::optional<process> dup() noexcept;
        process_id get_id() const noexcept;
        process_handle native_handle() const noexcept;
        void close() noexcept;
        bool is_running() noexcept;
        bool kill(int signum) noexcept;
        std::optional<uint32_t> wait() noexcept;
        bool resume() noexcept;

    private:
        process_handle hProcess;
        process_handle hThread;
        process_id dwProcessId;
        process_id dwThreadId;
    };

    struct args_t {
        void push(zstring_view v);
        void push(std::wstring&& v);
        std::wstring& operator[](size_t i) noexcept {
            return data_[i];
        }
        std::wstring const& operator[](size_t i) const noexcept {
            return data_[i];
        }
        size_t size() const noexcept {
            return data_.size();
        }

    private:
        std::vector<std::wstring> data_;
    };

    class spawn {
        friend class process;

    public:
        spawn() noexcept;
        ~spawn();
        void search_path() noexcept;
        void set_console(console type) noexcept;
        bool hide_window() noexcept;
        void suspended() noexcept;
        void detached() noexcept;
        void redirect(stdio type, file_handle h) noexcept;
        void env(environment&& env) noexcept;
        bool exec(const args_t& args, const wchar_t* cwd);

    private:
        void do_duplicate_shutdown() noexcept;

    private:
        environment env_ = nullptr;
        STARTUPINFOW si_;
        process pi_;
        DWORD flags_         = 0;
        console console_     = console::eInherit;
        bool inherit_handle_ = false;
        bool search_path_    = false;
        bool detached_       = false;
    };
}
