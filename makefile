compile: main.c
	gcc -o mm main.c xml.c -I /usr/include/libxml2 -lxml2
run: ./mm
	./mm -o ../outputs/testcase1 -v variables.xml -t template.xml
clean:
	find . -type f | xargs touch
	rm -rf $(OBJS)

