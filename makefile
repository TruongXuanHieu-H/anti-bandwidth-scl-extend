# ================================
# Project directories
# ================================
SRCDIR  := src
OBJDIR  := build/obj
BINDIR  := build

TARGET  := $(BINDIR)/abw_enc

# ================================
# Compiler
# ================================
CXX       := g++
STANDARD  := -std=c++11

# ================================
# Compiler flags
# ================================
IGNORE_ASSERTVARS := -Wno-unused-but-set-variable
CXXFLAGS := -Wall -Wextra -Werror -O3 -DNDEBUG $(STANDARD)

# ================================
# CaDiCaL
# ================================
CADICAL_FOLDER	?= cadical-1.2.1
CADICAL_INC     := ./$(CADICAL_FOLDER)
CADICAL_LIB_DIR := ./$(CADICAL_FOLDER)
CADICAL_LIB     := -lcadical

INCLUDES := -I$(CADICAL_INC)

# ================================
# Source files
# ================================
SOURCES := \
	main.cpp \
	utils.cpp \
	math_extension.cpp \
	reduced_encoder.cpp \
	sequential_encoder.cpp \
	product_encoder.cpp \
	duplex_encoder.cpp \
	scl_encoder.cpp \
	encoder.cpp \
	bdd.cpp \
	clause_cont.cpp \
	cadical_clauses.cpp \
	antibandwidth_encoder.cpp \
	abp_encoder.cpp

# ================================
# Object files
# ================================
OBJECTS := $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# ================================
# Default target
# ================================
.PHONY: all
all: $(TARGET)

# ================================
# Link
# ================================
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -L$(CADICAL_LIB_DIR) $(CADICAL_LIB) -o $@

# ================================
# Compile rules
# ================================
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Special flags for specific files
$(OBJDIR)/bdd.o: CXXFLAGS += $(IGNORE_ASSERTVARS)
$(OBJDIR)/duplex_encoder.o: CXXFLAGS += $(IGNORE_ASSERTVARS)

# ================================
# Clean
# ================================
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET) *.a *~ *.out

# ================================
# Archive
# ================================
.PHONY: tar
tar:
	tar cfv abw_enc.tar \
		main.cpp \
		makefile \
		$(SRCDIR) \
		$(CADICAL_LIB_DIR)/*.a \
		$(CADICAL_LIB_DIR)/*.hpp