#!/usr/bin/env python
from mpi4py import MPI
import csv


comm = MPI.COMM_WORLD
rank = comm.Get_rank()
_sends = 50000
_package = 1600
head = True

def normal(package, sends, csvfile):
    comm.Barrier()
    time = MPI.Wtime()
    if rank == 0:
        for i in range(sends):
            data = bytearray(package)
            comm.send(data, dest=1)
            data = comm.recv(source=1)

    elif rank == 1:
        for i in range(sends):
            data = bytearray(package)
            data = comm.recv(source=0)
            comm.send(data, dest=0)

        time = MPI.Wtime() - time
        capacity = float(2 * package * sends * 8) / (float(1000000) * time)
        delay = time / (float(sends) * 2)
        print(str(capacity) + "Mb/s")
        print(str(time / float(sends) * 2) + "s delay")

        spamwriter = csv.writer(csvfile, delimiter=',',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        spamwriter.writerow([package, sends, capacity, delay])


def buffer(package, sends, csvfile):
    buffer_size = 5 * package
    buf = bytearray([0] * buffer_size)
    comm.Barrier()
    time = MPI.Wtime()
    if rank == 0:
        for i in range(sends):
            data = bytearray(package)
            comm.Recv(data, source=1, tag=11)
            if i % (buffer_size / package) == 0:
                # flushing the buffer
                MPI.Detach_buffer()
                MPI.Attach_buffer(buf)
            comm.Send(data, dest=1, tag=11)


    elif rank == 1:
        for i in range(sends):
            data = bytearray(package)
            if i % (buffer_size / package) == 0:
                MPI.Detach_buffer()
                MPI.Attach_buffer(buf)
            comm.Send(data, dest=0, tag=11)
            comm.Recv(data, source=0, tag=11)

        MPI.Detach_buffer()
        time = MPI.Wtime() - time
        capacity = float(2 * package * sends * 8) / (float(1000000) * time)
        delay = time / (float(sends) * 2)
        print(str(capacity) + "Mb/s")
        print(str(delay)+ "s delay")

        spamwriter = csv.writer(csvfile, delimiter=',',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
        spamwriter.writerow([package, sends, capacity, delay])

csvfile = open('results/results_sync_package_2.csv', 'a')

spamwriter = csv.writer(csvfile, delimiter=',',
                        quotechar='|', quoting=csv.QUOTE_MINIMAL)
if head:
    spamwriter.writerow(['Package size[B]', 'Number of sends', 'Performance[Mb/s]', 'Delay[s]'])
    head = False

for i in range(20):
    normal((i+1) * _package,  _sends, csvfile)

csvfile = open('results/results_buffer_package_2.csv', 'a')

spamwriter = csv.writer(csvfile, delimiter=',',
                        quotechar='|', quoting=csv.QUOTE_MINIMAL)
head = True
if head:
    spamwriter.writerow(['Package size[B]', 'Number of sends', 'Performance[Mb/s]', 'Delay[s]'])
    head = False

for i in range(20):
    buffer((i+1) * _package,  _sends, csvfile)
