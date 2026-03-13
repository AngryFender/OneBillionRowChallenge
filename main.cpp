#include "datagenerator.h"
#include "naiveparser.h"
#include "mmparser.h"
#include "Strategies/flyweight.h"
#include "Strategies/multithreadspawn.h"
#include "Strategies/multithreadspawnlockfree.h"
#include "Strategies/multithreadspawnlockfreedoublemap.h"
#include "Strategies/parentthread.h"
#include "Strategies/singlethreadspawn.h"
#include <sstream>

#include "randomdatagenerator.h"

int main()
{
    NaiveParser naive(DATA_FILE_PATH);
    naive.start();
    {
        MMParser parent_thread_mm_parser(DATA_FILE_PATH, std::make_unique<ParentThread>());
        parent_thread_mm_parser.start();
    }

    {
        MMParser single_thread_mm_parser(DATA_FILE_PATH, std::make_unique<SingleThreadSpawn>());
        single_thread_mm_parser.start();
    }

    for (int t = 1; t <= 32; t = t * 2)
    {
        MMParser multi_thread_mm_parser(DATA_FILE_PATH, std::make_unique<MultiThreadSpawn>(t));
        multi_thread_mm_parser.start();
    }


    for (int t = 4; t <=32; t = t * 2)
    {
        MMParser multi_thread_mm_parser(DATA_FILE_PATH, std::make_unique<MultiThreadSpawnLockFree>(t, 4194304));
        multi_thread_mm_parser.start();
    }

    for (int t = 4; t <= 32; t = t * 2)
    {
        MMParser multi_thread_mm_parser(
            DATA_FILE_PATH, std::make_unique<MultiThreadSpawnLockFreeDoubleMap>(t, 4194304));
        multi_thread_mm_parser.start();
    }

    // DataGenerator dataGen(DATA_FILE_PATH);
    // dataGen.generate(1000000000);

    // RandomDataGenerator randomData(DATA_INPUT_FILE_PATH, DATA_OUTPUT_FILE_PATH);
    // randomData.generate(100);
    // randomData.generate(1000);
    // randomData.generate(1000000000);

    return 0;
}
