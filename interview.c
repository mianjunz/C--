/**
 * 嵌入式开发实习生面试 —— 常考手撕代码题（最小化指针版本）
 *
 * 设计原则：
 *   - 尽量避免指针参数，优先用返回值或全局/静态变量
 *   - 对固定长度的数据使用数组 + 索引
 *   - 链表/树等本质上需要指针的数据结构注明"必须指针"
 *
 * 涵盖：字符串 / 整数数组 / 位运算 / 排序 / 循环缓冲区 / 大小端
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* ================================================================
 * 1. 字符串长度计算（基于数组索引，不用指针遍历）
 *    思路：用整数索引替代指针遍历
 * ================================================================ */
size_t my_strlen_noptr(const char str[256])
{
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/* ================================================================
 * 2. 字符串拷贝（基于数组索引）
 *    返回拷贝的字符数（含 '\0'）
 * ================================================================ */
size_t my_strcpy_noptr(char dst[256], const char src[256])
{
    size_t i = 0;
    while ((dst[i] = src[i]) != '\0') {
        i++;
    }
    return i + 1;   /* 包括 '\0' */
}

/* ================================================================
 * 3. 内存块拷贝（基于数组和索引）
 *    假设固定长度 64 字节，避免 void* 指针
 * ================================================================ */
#define COPY_SIZE 64U

void my_memcpy_noptr(uint8_t dst[COPY_SIZE], const uint8_t src[COPY_SIZE], size_t n)
{
    if (n > COPY_SIZE) n = COPY_SIZE;
    for (size_t i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

/* ================================================================
 * 4. 内存块移动（处理重叠，用索引）
 * ================================================================ */
void my_memmove_noptr(uint8_t dst[COPY_SIZE], const uint8_t src[COPY_SIZE], size_t n)
{
    if (n > COPY_SIZE) n = COPY_SIZE;

    if (dst < src) {
        /* 正向拷贝 */
        for (size_t i = 0; i < n; i++) {
            dst[i] = src[i];
        }
    } else if (dst > src) {
        /* 反向拷贝，避免覆盖未读数据 */
        for (int i = (int)n - 1; i >= 0; i--) {
            dst[i] = src[i];
        }
    }
}

/* ================================================================
 * 5. 位运算：统计一个 uint32_t 中 1 的个数（Brian Kernighan 算法）
 *    每次 n &= (n-1) 消去最低位的 1
 * ================================================================ */
int count_ones(uint32_t n)
{
    int cnt = 0;
    while (n) {
        n &= (n - 1U);
        cnt++;
    }
    return cnt;
}

/* ================================================================
 * 6. 位运算：判断大小端
 *    小端：低地址存低字节；大端：低地址存高字节
 * ================================================================ */
int is_little_endian(void)
{
    uint16_t val = 0x0001U;
    return *(const uint8_t *)&val == 0x01U;  /* 1 = 小端，0 = 大端 */
}

/* ================================================================
 * 7. 位运算：三种方式交换两个整数（不用指针，用返回值）
 * ================================================================ */

/* 方式 1: 返回较小值，较大值存全局变量（不推荐） */
static int g_swap_larger = 0;

int swap_return_smaller(int a, int b)
{
    if (a < b) {
        g_swap_larger = b;
        return a;
    } else {
        g_swap_larger = a;
        return b;
    }
}

/* 方式 2: 返回结构体，包含两个交换后的值 */
typedef struct {
    int smaller;
    int larger;
} SwapResult;

SwapResult swap_struct(int a, int b)
{
    SwapResult res;
    if (a < b) {
        res.smaller = a;
        res.larger  = b;
    } else {
        res.smaller = b;
        res.larger  = a;
    }
    return res;
}

/* 方式 3: XOR 交换（不可避免用指针） */
void swap_xor_ptr(int *a, int *b)
{
    if (a != b) {
        *a ^= *b;
        *b ^= *a;
        *a ^= *b;
    }
}

/* ================================================================
 * 8. 反转整数数组（in-place，不用指针）
 * ================================================================ */
void reverse_array(int arr[32], int n)
{
    int lo = 0, hi = n - 1;
    while (lo < hi) {
        int tmp    = arr[lo];
        arr[lo]    = arr[hi];
        arr[hi]    = tmp;
        lo++;
        hi--;
    }
}

/* ================================================================
 * 9. 单向链表反转
 *    注：链表本质上需要指针，此处标注为"必须指针"
 *    面试时会问"如何避免用指针"？答案：链表无法避免，因为需要动态连接
 * ================================================================ */
typedef struct Node {
    int          data;
    struct Node *next;   /* 必须指针 */
} Node;

/* 【必须指针】链表反转 */
Node *list_reverse(Node *head)
{
    Node *prev = NULL;
    Node *curr = head;
    while (curr != NULL) {
        Node *next = curr->next;
        curr->next = prev;
        prev       = curr;
        curr       = next;
    }
    return prev;
}

/* 【推荐】静态数组模拟链表，避免指针
 *  用数组的下标代替指针
 */
#define MAX_NODES 16U

typedef struct {
    int data;
    int next;   /* 下一个节点的数组下标，-1 表示 NULL */
} ArrayNode;

static ArrayNode nodes[MAX_NODES];
static int free_idx = 0;

/* 返回新节点的下标 */
int array_list_alloc(int data)
{
    if (free_idx >= MAX_NODES) return -1;
    nodes[free_idx].data = data;
    nodes[free_idx].next = -1;
    return free_idx++;
}

/* 反转链表（基于数组下标） */
int array_list_reverse(int head)
{
    int prev = -1, curr = head;
    while (curr != -1) {
        int next_idx = nodes[curr].next;
        nodes[curr].next = prev;
        prev = curr;
        curr = next_idx;
    }
    return prev;
}

/* ================================================================
 * 10. 快速排序（迭代版本 + 栈，避免递归中的隐藏指针）
 *
 * 传统快排用递归，汇编层面涉及大量栈帧操作。
 * 改用显式栈的迭代版本，逻辑清晰。
 * ================================================================ */
typedef struct {
    int data[32];
    int size;
} Stack;

void stack_push(Stack *s, int val)
{
    if (s->size < 32) s->data[s->size++] = val;
}

int stack_pop(Stack *s)
{
    return (s->size > 0) ? s->data[--s->size] : -1;
}

int stack_empty(const Stack *s)
{
    return s->size == 0;
}

static void quicksort_iter_helper(int arr[32], int lo, int hi)
{
    Stack s = {0};
    stack_push(&s, lo);
    stack_push(&s, hi);

    while (!stack_empty(&s)) {
        hi = stack_pop(&s);
        lo = stack_pop(&s);
        if (lo >= hi) continue;

        /* --- partition --- */
        int pivot = arr[hi];
        int i     = lo - 1;
        for (int j = lo; j < hi; j++) {
            if (arr[j] <= pivot) {
                i++;
                int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            }
        }
        int tmp = arr[i + 1]; arr[i + 1] = arr[hi]; arr[hi] = tmp;
        int p   = i + 1;

        /* 入栈待处理的左右段 */
        if (lo < p - 1) {
            stack_push(&s, lo);
            stack_push(&s, p - 1);
        }
        if (p + 1 < hi) {
            stack_push(&s, p + 1);
            stack_push(&s, hi);
        }
    }
}

void my_quicksort_noptr(int arr[32], int n)
{
    if (arr && n > 1) quicksort_iter_helper(arr, 0, n - 1);
}

/* ================================================================
 * 11-ext. 冒泡排序（Bubble Sort）
 *
 * 思路：每轮从头到尾相邻两两比较，将较大值「冒泡」到末尾；
 *       n-1 轮后数组有序。
 *       优化：若某轮没有发生任何交换，说明数组已有序，提前退出。
 * 时间复杂度：最好（已有序）O(n)，平均/最坏 O(n²)
 * 空间复杂度：O(1)，原地排序
 * 适用场景：数据量小，或几乎有序；面试中常用于对比快排的局限性
 * ================================================================ */
void bubble_sort(int *arr, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;   /* 本轮是否发生交换的标志（优化提前退出） */
        /* 每轮结束后，最大的 i+1 个元素已就位，无需再比较 */
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                int tmp    = arr[j];
                arr[j]     = arr[j + 1];
                arr[j + 1] = tmp;
                swapped    = 1;
            }
        }
        if (!swapped) break;   /* 本轮无交换 → 已有序，提前退出 */
    }
}

/* ================================================================
 * 11. 循环缓冲区（Ring Buffer）
 *     嵌入式串口 / DMA 中极常用的数据结构
 * ================================================================ */
#define RING_BUF_SIZE 8U   /* 必须是 2 的幂，方便用位掩码取模 */

typedef struct {
    uint8_t  buf[RING_BUF_SIZE];
    uint32_t head;   /* 读指针 */
    uint32_t tail;   /* 写指针 */
} RingBuf;

/* 返回 1 表示成功，0 表示缓冲区已满 */
int ringbuf_push(RingBuf *rb, uint8_t data)
{
    uint32_t next = (rb->tail + 1U) & (RING_BUF_SIZE - 1U);
    if (next == rb->head) return 0;   /* 满 */
    rb->buf[rb->tail] = data;
    rb->tail          = next;
    return 1;
}

/* 返回 1 表示成功，0 表示缓冲区为空 */
int ringbuf_pop(RingBuf *rb, uint8_t *out)
{
    if (rb->head == rb->tail) return 0;   /* 空 */
    *out     = rb->buf[rb->head];
    rb->head = (rb->head + 1U) & (RING_BUF_SIZE - 1U);
    return 1;
}

/* ================================================================
 * 12. 二分查找（在有序数组中查找目标值，返回下标，未找到返回 -1）
 * ================================================================ */
int binary_search(const int *arr, int n, int target)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;   /* 防止 (lo+hi) 溢出 */
        if (arr[mid] == target) return mid;
        if (arr[mid]  < target) lo = mid + 1;
        else                    hi = mid - 1;
    }
    return -1;
}

/* ================================================================
 * main：用于在本地验证上述函数
 * ================================================================ */
static int main(void)
{
    /* --- 1. strlen --- */
    printf("[strlen] \"hello\" = %zu\n", my_strlen("hello"));

    /* --- 5. count_ones --- */
    printf("[count_ones] 0xABU = %d\n", count_ones(0xABU)); /* 1010 1011 -> 5 */

    /* --- 6. endian --- */
    printf("[endian] %s\n", is_little_endian() ? "Little-Endian" : "Big-Endian");

    /* --- 9. list_reverse --- */
    Node n3 = {3, NULL};
    Node n2 = {2, &n3};
    Node n1 = {1, &n2};
    Node *head = list_reverse(&n1);
    printf("[list_reverse] ");
    for (Node *p = head; p; p = p->next) printf("%d ", p->data);
    printf("\n");  /* 期望: 3 2 1 */

    /* --- 10. quicksort --- */
    int arr[] = {5, 3, 8, 1, 9, 2};
    int len   = (int)(sizeof(arr) / sizeof(arr[0]));
    my_quicksort(arr, len);
    printf("[quicksort]   ");
    for (int i = 0; i < len; i++) printf("%d ", arr[i]);
    printf("\n");

    /* --- 11-ext. bubble_sort --- */
    int arr2[] = {5, 3, 8, 1, 9, 2};
    int len2   = (int)(sizeof(arr2) / sizeof(arr2[0]));
    bubble_sort(arr2, len2);
    printf("[bubble_sort] ");
    for (int i = 0; i < len2; i++) printf("%d ", arr2[i]);
    printf("\n");

    /* --- 11. ring buffer --- */
    RingBuf rb = {.head = 0, .tail = 0};
    ringbuf_push(&rb, 0xAAU);
    ringbuf_push(&rb, 0xBBU);
    uint8_t val;
    ringbuf_pop(&rb, &val);
    printf("[ring_buf] pop = 0x%02X\n", val);  /* 期望: 0xAA */

    /* --- 12. binary_search --- */
    int sorted[] = {1, 3, 5, 7, 9, 11};
    printf("[binary_search] target=7, idx=%d\n",
           binary_search(sorted, 6, 7));  /* 期望: 3 */

    return 0;
}
