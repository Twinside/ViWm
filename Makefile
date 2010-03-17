CC    := gcc
FLAGS := -Wall -Wextra -s
OBJ	  := Layouter.o \
			LayoutTree.o \
			State.o \
			TilledWindow.o \
			ViWm.o \
			ViWm.win32.o \
			ActionFunctors.o \
			Actions\TreeAction.o \
			Layouter\ManualVimLayout.o \

viwmtest.exe: $(OBJ)
	#$(CC) ${FLAGS} -o $@ $<

%.o: %.cpp
	$(CC) ${FLAGS} -c $<

%.o: %.cc
	$(CC) ${FLAGS} -c $<

