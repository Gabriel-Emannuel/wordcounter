from os import listdir

DIRETORIO_DATA = "dataset"

def pega_diretorios():
    return listdir(DIRETORIO_DATA)

def pega_arquivos(diretorio: str) -> str:
    return listdir(diretorio)

def conta_palavra(arquivo: str) -> str:
    with open(arquivo, 'r') as f:
        return len(f.read().strip().split())
    
if __name__ == "__main__":
    contador = 0
    
    all_files = [ 
        f"{DIRETORIO_DATA}/{diretorio}/{file}" for diretorio in pega_diretorios() 
        for file in pega_arquivos(f"{DIRETORIO_DATA}/{diretorio}") 
        ]
    
    for file in all_files:
        contador += conta_palavra(file)
    
    print(f"Existem {contador} palavras no dataset")