from os.path import exists

from os import mkdir

from random import randint

DIRETORIO_DATA = "dataset"

PREFIXO_ARQUIVOS = "file_"

QUANTIDADE_DATASETS = 12

QUANTIDADE_ARQUIVOS = 1000

PALAVRA = "abc"

def gera_arquivo(caminho: str) -> None:
    
    palavras = [PALAVRA for _ in range(randint(10,100))]

    with open(caminho, "w") as f:
        f.write(" ".join(palavras))

def gera_diretorio(caminho: str) -> None:
    mkdir(caminho)

if __name__ == "__main__":
    if not exists(DIRETORIO_DATA):
        gera_diretorio(DIRETORIO_DATA)
        for dir in range(QUANTIDADE_DATASETS):
            gera_diretorio(f"{DIRETORIO_DATA}/{dir}")
            for file_number in range(QUANTIDADE_ARQUIVOS):
                gera_arquivo(
                    f"{DIRETORIO_DATA}/{dir}/{PREFIXO_ARQUIVOS}{file_number}")