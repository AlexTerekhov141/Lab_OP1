/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов CEcoLab1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab1.h"
/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoLab1
 * </описание>
 *
 */


static int16_t ECOCALLMETHOD CEcoLab1_QueryInterface(/* in */ IEcoLab1Ptr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателей */
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoLab1) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoLab1
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_AddRef(/* in */ IEcoLab1Ptr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Swap
 * </сводка>
 *
 * <описание>
 *   Меняет местами два значения типа int32_t, переданные по указателю.
 *   Используется как вспомогательная операция при перестановках элементов.
 *   Проверяет аргументы на NULL и возвращает код ошибки при некорректных
 *   входных данных.
 * </описание>
 */
static int32_t ECOCALLMETHOD CEcoLab1_Swap(/* in */ IEcoLab1Ptr_t me, /* inout */ int32_t* a, /* inout */ int32_t* b) {
    int32_t tmp;

    /* Проверка указателей */
    if (me == 0 || a == 0 || b == 0) {
        return -1; /* ERR_ECO_POINTER */
    }

    tmp = *a;
    *a = *b;
    *b = tmp;

    return 0; /* успех */
}

/*
 *
 * <сводка>
 *   Функция get_child_trees
 * </сводка>
 *
 * <описание>
 *   Вычисляет индексы корней правого и левого дочерних деревьев Леонардо
 *   и соответствующие порядковые номера (k) для заданного узла дерева.
 *   Нужна при разложении дерева Леонардо на дочерние поддеревья.
 * </описание>
 *
 *
 */
void get_child_trees(int i, int k, int *leo_nums, int *t_r, int *k_r, int *t_l, int *k_l) {
    *t_r = i - 1;
    *k_r = k - 2;
    *t_l = *t_r - leo_nums[*k_r];
    *k_l = k - 1;
}

/*
 *
 * <сводка>
 *   Функция leonardo_numbers
 * </сводка>
 *
 * <описание>
 *   Построение последовательности чисел Леонардо до значения hi.
 *   Выделяет и возвращает массив чисел Леонардо; размер возвращается
 *   через параметр size. Память под массив выделяется через malloc,
 *   вызывающая сторона обязана вызвать free.
 * </описание>
 *
 */
int* leonardo_numbers(int hi, int *size) {
    int capacity = 16;
    int *numbers = (int*)malloc(capacity * sizeof(int));
    int a = 1, b = 1;
    int count = 0;
	int next = 0;
    while (a <= hi) {
        if (count >= capacity) {
            capacity *= 2;
            numbers = (int*)realloc(numbers, capacity * sizeof(int));
        }
        numbers[count++] = a;
        next = a + b + 1;
        a = b;
        b = next;
    }
    *size = count;
    return numbers;
}

/*
 *
 * <сводка>
 *   Функция restore_heap
 * </сводка>
 *
 * <описание>
 *   Восстанавливает свойство кучи (heap invariant) для дерева Леонардо,
 *   корень которого находится в позиции i в массиве arr.
 *   Используется в фазах построения и разрушения леса деревьев Леонардо.
 *   Функция опирается на массив leo_nums с числами Леонардо и на стек
 *   (heap[]) с порядковыми номерами деревьев.
 * </описание>
 *
 *
 */
void restore_heap(IEcoLab1Ptr_t me, int *arr, int i, int *heap, int heap_size, int *leo_nums) {
    int current = heap_size - 1;
    int k = heap[current];
	int j, t_r, k_r, t_l, k_l;
	CEcoLab1* pCMe = (CEcoLab1*)me;
    while (current > 0) {
        j = i - leo_nums[k];
        if (arr[j] > arr[i] && (k < 2  || (arr[j] > arr[i - 1] && arr[j] > arr[i - 2]))) {
            CEcoLab1_Swap((IEcoLab1*)pCMe, &arr[i], &arr[j]);
            i = j;
            current--;
            k = heap[current];
        } else {
            break;
        }
    }

    while (k >= 2) {
        t_r, k_r, t_l, k_l;
        get_child_trees(i, k, leo_nums, &t_r, &k_r, &t_l, &k_l);
        if (arr[i] < arr[t_r] || arr[i] < arr[t_l]) {
            if (arr[t_r] > arr[t_l]) {
                CEcoLab1_Swap((IEcoLab1*)pCMe, &arr[i], &arr[t_r]);
                i = t_r;
                k = k_r;
            } else {
                CEcoLab1_Swap((IEcoLab1*)pCMe, &arr[i], &arr[t_l]);
                i = t_l;
                k = k_l;
            }
        } else {
            break;
        }
    }
}

/*
 *
 * <сводка>
 *   Функция SmoothSort
 * </сводка>
 *
 * <описание>
 *   Реализует сортировку массива целых чисел по алгоритму D. H. Дейкстры
 *   (Smoothsort) с использованием леса деревьев Леонардо. Сортировка выполняется
 *   in-place (в переданном массиве arr) по возрастанию.
 *
 *   Алгоритм состоит из двух фаз:
 *     1. Построение леса деревьев Леонардо (формирование структуры heap[])
 *        и поддержание инварианта (restore_heap) при добавлении нового
 *        элемента (фаза "build").
 *     2. Разбор леса (разложение корней деревьев) с восстановлением
 *        инварианта и последовательным извлечением элементов (фаза "drain").
 * </описание>
 *
 */
static int32_t ECOCALLMETHOD CEcoLab1_SmoothSort(/* in */ IEcoLab1Ptr_t me, /* inout */ int32_t* arr, /* in */ int32_t n) {
	CEcoLab1* pCMe = (CEcoLab1*)me;
	int leo_size = 0;
    int *leo_nums = leonardo_numbers(n, &leo_size);

    int *heap = (int*)malloc(n * sizeof(int));
    int heap_size = 0;
	int i = 0, k = 0, t_r, k_r, t_l, k_l;

    for (i = 0; i < n; i++) {
        if (heap_size >= 2 && heap[heap_size - 2] == heap[heap_size - 1] + 1) {
            heap_size--;
            heap[heap_size - 1] += 1;
        } else {
            if (heap_size >= 1 && heap[heap_size - 1] == 1) {
                heap[heap_size++] = 0;
            } else {
                heap[heap_size++] = 1;
            }
        }
        restore_heap((IEcoLab1*)pCMe, arr, i, heap, heap_size, leo_nums);
    }

    for (i = n - 1; i >= 0; i--) {
        if (heap_size == 0) break;
        if (heap[heap_size - 1] < 2) {
            heap_size--;
        } else {
            k = heap[--heap_size];
            t_r, k_r, t_l, k_l;
            get_child_trees(i, k, leo_nums, &t_r, &k_r, &t_l, &k_l);
            heap[heap_size++] = k_l;
            restore_heap((IEcoLab1*)pCMe, arr, t_l, heap, heap_size, leo_nums);
            heap[heap_size++] = k_r;
            restore_heap((IEcoLab1*)pCMe, arr, t_r, heap, heap_size, leo_nums);
        }
    }

    free(heap);
    free(leo_nums);

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoLab1
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_Release(/* in */ IEcoLab1Ptr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    /* Уменьшение счетчика ссылок на компонент */
    --pCMe->m_cRef;

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoLab1((IEcoLab1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция MyFunction
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_MyFunction(/* in */ IEcoLab1Ptr_t me, /* in */ char_t* Name, /* out */ char_t** copyName, int32_t*  arr, int32_t n) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    int16_t index = 0;
	int i = 0;
    /* Проверка указателей */
    if (me == 0 || Name == 0 || copyName == 0) {
        return ERR_ECO_POINTER;
    }

    /* Копирование строки */
    while(Name[index] != 0) {
        index++;
    }
    pCMe->m_Name = (char_t*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, index + 1);
    index = 0;
    while(Name[index] != 0) {
        pCMe->m_Name[index] = Name[index];
        index++;
    }
    *copyName = pCMe->m_Name;

    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция Init
 * </сводка>
 *
 * <описание>
 *   Функция инициализации экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD initCEcoLab1(/*in*/ IEcoLab1Ptr_t me, /* in */ struct IEcoUnknown *pIUnkSystem) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    IEcoInterfaceBus1* pIBus = 0;
    int16_t result = -1;

    /* Проверка указателей */
    if (me == 0 ) {
        return result;
    }

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    /* Проверка указателей */
    if (me == 0 ) {
        return result;
    }

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;



    /* Освобождение */
    pIBus->pVTbl->Release(pIBus);

    return result;
}

/* Create Virtual Table IEcoLab1 */
IEcoLab1VTbl g_x277FC00C35624096AFCFC125B94EEC90VTbl = {
    CEcoLab1_QueryInterface,
    CEcoLab1_AddRef,
    CEcoLab1_Release,
    CEcoLab1_MyFunction,
CEcoLab1_Swap,
CEcoLab1_SmoothSort
};

/*
 *
 * <сводка>
 *   Функция Create
 * </сводка>
 *
 * <описание>
 *   Функция создания экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD createCEcoLab1(/* in */ IEcoUnknown* pIUnkSystem, /* in */ IEcoUnknown* pIUnkOuter, /* out */ IEcoLab1** ppIEcoLab1) {
    int16_t result = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    CEcoLab1* pCMe = 0;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    /* Проверка указателей */
    if (ppIEcoLab1 == 0 || pIUnkSystem == 0) {
        return result;
    }

    /* Получение системного интерфейса приложения */
    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void **)&pISys);

    /* Проверка */
    if (result != 0 && pISys == 0) {
        return result;
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    /* Получение идентификатора компонента для работы с памятью */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Получение интерфейса распределителя памяти */
    pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 && pIMem == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
        pISys->pVTbl->Release(pISys);
        return result;
    }

    /* Выделение памяти для данных экземпляра */
    pCMe = (CEcoLab1*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoLab1));

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = pISys;

    /* Сохранение указателя на интерфейс для работы с памятью */
    pCMe->m_pIMem = pIMem;

    /* Установка счетчика ссылок на компонент */
    pCMe->m_cRef = 1;

    /* Создание таблицы функций интерфейса IEcoLab1 */
    pCMe->m_pVTblIEcoLab1 = &g_x277FC00C35624096AFCFC125B94EEC90VTbl;

    /* Инициализация данных */
    pCMe->m_Name = 0;

    /* Возврат указателя на интерфейс */
    *ppIEcoLab1 = (IEcoLab1*)pCMe;

    /* Освобождение */
    pIBus->pVTbl->Release(pIBus);

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Delete
 * </сводка>
 *
 * <описание>
 *   Функция освобождения экземпляра
 * </описание>
 *
 */
void ECOCALLMETHOD deleteCEcoLab1(/* in */ IEcoLab1* pIEcoLab1) {
    CEcoLab1* pCMe = (CEcoLab1*)pIEcoLab1;
    IEcoMemoryAllocator1* pIMem = 0;

    if (pIEcoLab1 != 0 ) {
        pIMem = pCMe->m_pIMem;
        /* Освобождение */
        if ( pCMe->m_Name != 0 ) {
            pIMem->pVTbl->Free(pIMem, pCMe->m_Name);
        }
        if ( pCMe->m_pISys != 0 ) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}