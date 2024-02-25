# writing a py file that writes Hello World to its own file
import sys

if __name__ == '__main__':
    with open(sys.argv[0], 'w') as f:
        f.write('print("Hello World")\n')
    print('Hello World\nHello World')
    print('lorem ipsum')