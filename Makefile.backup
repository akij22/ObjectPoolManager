# Compilatore e flag
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Nome dell'eseguibile
TARGET   = objmem

# ATTENZIONE: nei sorgenti metti solo i .cpp, NON gli header!
SRC = testObjPool.cpp

# Da .cpp a .o
OBJ = $(SRC:.cpp=.o)

# Target di default
all: $(TARGET)

# Link: crea l'eseguibile dagli oggetti
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compila i .cpp in .o
# Metto ObjectPoolManager.hh come dipendenza così se cambia, ricompila
%.o: %.cpp ObjectPoolManager.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Esegui il programma
run: $(TARGET)
	./$(TARGET)

# Pulisci i file generati
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean run

