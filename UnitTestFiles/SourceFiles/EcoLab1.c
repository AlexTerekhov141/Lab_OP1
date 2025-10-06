/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include <time.h>
/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */

int __cdecl compare_ints(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

void test_sort_speed(IEcoLab1* pIEcoLab1, IEcoMemoryAllocator1* pIMem, int32_t n) {
    int i;
    int32_t* arr1 = (int32_t*)pIMem->pVTbl->Alloc(pIMem, n * sizeof(int32_t));
    int32_t* arr2 = (int32_t*)pIMem->pVTbl->Alloc(pIMem, n * sizeof(int32_t));
	double smooth_time;
	double qsort_time;
	clock_t start;
	clock_t end;
    
    srand((unsigned int)time(0));
    for (i = 0; i < n; i++) {
        int val = rand();
        arr1[i] = val;
        arr2[i] = val;
    }

    /* SmoothSort */
    start = clock();
    pIEcoLab1->pVTbl->SmoothSort(pIEcoLab1, arr1, n);
    end = clock();
    smooth_time = (double)(end - start) / CLOCKS_PER_SEC;

    /* qsort */
    start = clock();
    qsort(arr2, n, sizeof(int32_t), compare_ints);
    end = clock();
    qsort_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Array size: %d\n", n);
    printf("SmoothSort time: %.6f sec\n", smooth_time);
    printf("qsort      time: %.6f sec\n", qsort_time);
    printf("-----------------------------\n");

    pIMem->pVTbl->Free(pIMem, arr1);
    pIMem->pVTbl->Free(pIMem, arr2);
}

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    char_t* name = 0;
    char_t* copyName = 0;
    /* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;
	int32_t n = 0;
	int32_t* arr = 0;
	int32_t arr_test[10] = {5, 2, 3, 4, 1, -1, 0, 100, 64, 73};
	int i = 0;
	int sizes[] = {10, 50, 100, 500, 1000, 5000, 10000, 25000, 50000, 75000, 100000};
    int count = sizeof(sizes) / sizeof(sizes[0]);

    /* Проверка и создание системного интрефейса */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 && pISys == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
            goto Release;
        }
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
#ifdef ECO_LIB
    /* Регистрация статического компонента для работы со списком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0 ) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
#endif
    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 || pIMem == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
        goto Release;
    }

    /* Выделение блока памяти */
    name = (char_t *)pIMem->pVTbl->Alloc(pIMem, 10);

    /* Заполнение блока памяти */
    pIMem->pVTbl->Fill(pIMem, name, 'a', 9);


    /* Получение тестируемого интерфейса */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        /* Освобождение интерфейсов в случае ошибки */
        goto Release;
    }

	for (i = 0; i < count; i++) {
        test_sort_speed(pIEcoLab1, pIMem, sizes[i]);
    }
	result = pIEcoLab1->pVTbl->SmoothSort(pIEcoLab1, arr_test, 10);
	for (i = 0; i < 10; i++) {
        printf("%d ", arr_test[i]);
    }
	printf("\n");
    //result = pIEcoLab1->pVTbl->MyFunction(pIEcoLab1, name, &copyName, arr, n);
	system("Pause");

    /* Освлбождение блока памяти */
    pIMem->pVTbl->Free(pIMem, name);

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) {
        pIEcoLab1->pVTbl->Release(pIEcoLab1);
    }


    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}
