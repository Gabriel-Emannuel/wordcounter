from os import listdir
from threading import Thread, Semaphore

THREAD_LIMIT = 10

DIRETORIO_DATA = "dataset"

MUTEX = Semaphore(1)

contador = 0

def pega_diretorios():
    return listdir(DIRETORIO_DATA)

def pega_arquivos(diretorio: str) -> str:
    return listdir(diretorio)

def conta_palavra(arquivo: str) -> str:
    global contador
    
    with open(arquivo, 'r') as f:
        counter_stack = len(f.read().strip().split())

    MUTEX.acquire()
    contador += counter_stack
    MUTEX.release()

if __name__ == "__main__":

    all_files = [ 
        f"{DIRETORIO_DATA}/{diretorio}/{file}" for diretorio in pega_diretorios() 
        for file in pega_arquivos(f"{DIRETORIO_DATA}/{diretorio}") 
        ]
    
    argc = len(all_files)

    indice = 0

    while indice < argc:

        pool_size = argc - indice

        if pool_size > THREAD_LIMIT:
            pool_size = THREAD_LIMIT

        thread_pool = [
            Thread(target=conta_palavra, args=(file,)) 
            for file in all_files[indice:indice + pool_size]
            ]

        for thread in thread_pool:
            thread.start()

        for thread in thread_pool:
            thread.join()

        indice += pool_size

    print(f"Existem {contador} palavras no dataset")