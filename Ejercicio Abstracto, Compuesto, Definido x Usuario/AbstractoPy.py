class Pila:
    def __init__(self):
        self.datos = []
    def push(self, x):
        self.datos.append(x)
    def pop(self):
        return self.datos.pop()

p = Pila()
p.push(8)
p.push(10)
print(p.pop())
