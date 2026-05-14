#pragma once

#define LIVIO_ARK_DECLARE_SINGLETON(ClassName) \
private: \
    ClassName() = default; \
    ~ClassName() = default; \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete; \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete; \
public: \
    static ClassName& instance() { \
        static ClassName instance; \
        return instance; \
    }

namespace livio::ark::base {
template<typename T>
class Singleton {
public:
    static T &instance() {
        static T instance;
        return instance;
    }

    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton &) = delete;

    Singleton(Singleton &&) = delete;

    Singleton &operator=(Singleton &&) = delete;

protected:
    Singleton() = default;

    virtual ~Singleton() = default;
};
} // namespace livio::ark::base
