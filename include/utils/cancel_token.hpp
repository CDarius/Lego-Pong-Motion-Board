#pragma once

#include <vector>
#include <algorithm>
#include <mutex>

// Usage:
// EXECUTE_IF_CANCELLED(token, {
//     // code to execute if token.IsCancelled() is true
// });
#define IF_CANCELLED(token, block) \
    do { \
        if ((token).isCancelled()) { \
            block \
        } \
    } while(0)

class CancelToken {
private:
    static std::vector<CancelToken*> instances;
    static std::mutex instancesMutex;
    bool cancelled = false;

public:
    CancelToken();

    ~CancelToken();

    bool isCancelled() const;
    void cancel();

    static void cancelAll();
};