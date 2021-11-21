# Script utilizado para contagem dos vertices de cada parte do objeto
with open('testarossa.obj', 'r') as inputfile: 
    vertices = 0
    total = 0
    linhas = 0
    for line in inputfile: 
        if line.startswith("# usemtl") or line.startswith("o "): 
            total += vertices
            print(f"Vertices: {vertices} - Total: {total} Desc - {line}")
            vertices = 0
        elif line.startswith("f "): 
            n = len(line.split(" ")) - 1
            vertices += 3*(n-2)
        linhas += 1
    total += vertices
    print(f"Vertices: {vertices} - Total: {total} - Linhas processadas: {linhas}")
