CPP   := g++
FLAGS := -Wall -Wextra -s

OBJ	  := Layouter.o \
			LayoutTree.o \
			State.o \
			TilledWindow.o \
			Actions/TreeAction.o \
			Actions/Debug.o \
			Actions/MultiScreen.o \
			Actions/System.o \
			Layouter/ManualVimLayout.o

viwmtest.exe: $(OBJ)
	#$(CC) ${FLAGS} -o $@ $<

%.o: %.cpp
	$(CPP) ${FLAGS} -c $<

%.o: %.cc
	$(CPP) ${FLAGS} -c $<

Actions/%.o: Actions/%.cpp
	$(CPP) ${FLAGS} -c $<

Actions/%.o: Actions/%.cc
	$(CPP) ${FLAGS} -c $<

Layouter/%.o: Layouter/%.cc
	$(CPP) ${FLAGS} -c $<

Layouter/%.o: Layouter/%.cpp
	$(CPP) ${FLAGS} -c $<

