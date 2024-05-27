# ALTLI
hdrs = ipc.h
opts = -g -c

b_src = barberShop.c ipc.c
b_obj = barberShop.o ipc.o
c_src = customer.c ipc.c
c_obj = customer.o ipc.o
h_src = hairCutter.c ipc.c
h_obj = hairCutter.o ipc.o

all: barberShop customer hairCutter
	
barberShop: $(b_obj)
	gcc $(b_obj) -o barberShop
barberShop.o: $(b_src) $(hdrs)
	gcc $(opts) $(b_src)
customer: $(c_obj)
	gcc $(c_obj) -o customer
customer.o: $(c_src) $(hdrs)
	gcc $(opts) $(c_src)
hairCutter: $(h_obj)
	gcc $(h_obj) -o hairCutter
hairCutter.o: $(h_src) $(hdrs)
	gcc $(opts) $(h_src)

clean:
	rm barberShop customer hairCutter *.o

