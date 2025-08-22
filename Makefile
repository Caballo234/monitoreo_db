# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include
LDFLAGS = -lmysqlcppconn -lpthread

# Directorios
SRCDIR = src
BUILDDIR = build
TARGET = monitor_service

# Archivos fuente y objeto
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Regla principal
all: $(BUILDDIR)/$(TARGET)

# Regla para enlazar el ejecutable
$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# --- REGLA CORREGIDA ---
# Esta regla ahora usa las variables automáticas correctas
# $< : es el primer prerrequisito (el archivo .cpp)
# $@ : es el nombre del objetivo (el archivo .o)
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos compilados
clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean