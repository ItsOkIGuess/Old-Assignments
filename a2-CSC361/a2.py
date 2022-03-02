import sys
import struct
import basic_structures as bs

def checkConnection(tup,cons):
	connection = []
	flagcount = [0,0,0,0]
	for i in range(0,len(cons) - 1):
		
		if(tup[0] == cons[i].IP_Header.src_ip and tup[1] == cons[i].IP_Header.dst_ip and tup[2] == cons[i].TCP_Header.src_port and tup [3] == cons[i].TCP_Header.dst_port):
			connection.append(cons[i])
			flagcount[0] = flagcount[0] + cons[i].TCP_Header.flags["ACK"]
			flagcount[1] = flagcount[1] + cons[i].TCP_Header.flags["RST"]
			flagcount[2] = flagcount[2] + cons[i].TCP_Header.flags["SYN"]
			flagcount[3] = flagcount[3] + cons[i].TCP_Header.flags["FIN"]
		if(tup[1] == cons[i].IP_Header.src_ip and tup[0] == cons[i].IP_Header.dst_ip and tup[3] == cons[i].TCP_Header.src_port and tup [2] == cons[i].TCP_Header.dst_port):
			connection.append(cons[i])
			flagcount[0] = flagcount[0] + cons[i].TCP_Header.flags["ACK"]
			flagcount[1] = flagcount[1] + cons[i].TCP_Header.flags["RST"]
			flagcount[2] = flagcount[2] + cons[i].TCP_Header.flags["SYN"]
			flagcount[3] = flagcount[3] + cons[i].TCP_Header.flags["FIN"]
				
	return connection, flagcount


def main():
	if(len(sys.argv) ==1):
		print('please enter a file')
		return
	if(len(sys.argv) >2):
		print('Too many inputs')
		return
	x = 0
	timestart = 0
	file = str(sys.argv[1])
	#print(file)
	tcp = open(file, 'rb')
	data = tcp.read(24)
	list = []
	f = 0
	
	while(tcp):
		pack = bs.packet()
		list.append(pack)
		data = tcp.read(16)
		
		if len(data) == 0:
			break
		list[x].packet_No_set(x)
		list[x].timestamp_set(data[0:4],data[4:8],timestart)
		timestart = list[0].timestamp
		size = struct.unpack("ll", data[8:16])
		data = tcp.read(size[0])
		
		h = bs.IP_Header()
		h.get_header_len(data[14:15])
		h.get_total_len(data[16:18])
		h.get_IP(data[(6+h.ip_header_len):(10+h.ip_header_len)],data[(10+h.ip_header_len):(14+h.ip_header_len)])
		
		t = bs.TCP_Header()
		t.get_src_port(data[(14+h.ip_header_len):(16+h.ip_header_len)])
		t.get_dst_port(data[(16+h.ip_header_len):(18+h.ip_header_len)])
		t.get_seq_num(data[(18+h.ip_header_len):(22+h.ip_header_len)])
		t.get_ack_num(data[(22+h.ip_header_len):(26+h.ip_header_len)])
		t.get_data_offset(data[(26+h.ip_header_len):(27+h.ip_header_len)])
		t.get_flags(data[27+h.ip_header_len:28+h.ip_header_len])
		t.get_window_size(data[28+h.ip_header_len:29+h.ip_header_len],data[29+h.ip_header_len:30+h.ip_header_len])
		list[x].TCP_Header = t
		list[x].IP_Header = h
		x = x + 1
		
	list[0].timestamp = 0.000000
	numCons = []
	GRAVY = 0
	completecons = 0
	resetcons = 0
	incompletecons = 0
	packets = []
	Mpackets = 0
	duration = []
	Mduration = 0
	window = []
	Mwindow = 0
	rtt = []
	Mrtt = 0
	numcompletepackets = 0
	for i in range(1,len(list)-1):
		t = (list[i].IP_Header.src_ip, list[i].IP_Header.dst_ip, list[i].TCP_Header.src_port, list[i].TCP_Header.dst_port)
		if(t not in numCons):
			numCons.append(t)
			GRAVY = GRAVY + 0.5
			
	print("A) Total number of connections: " + str(GRAVY))
	print("_______________________________________________________________________________")
	print()
	
	tups = []
	num = 1
	flags = []
	print("B) Connections' details:")
	print("_______________________________________________________________________________")
	print()
	for i in range(0,len(list)-1):
		
		t = (list[i].IP_Header.src_ip, list[i].IP_Header.dst_ip, list[i].TCP_Header.src_port, list[i].TCP_Header.dst_port)
		tt = (list[i].IP_Header.dst_ip, list[i].IP_Header.src_ip, list[i].TCP_Header.dst_port, list[i].TCP_Header.src_port)
		if(t not in tups and tt not in tups):
			tups.append(t)
			tups.append(tt)
			connection,flags = checkConnection(t, list)

			print("Connection: " + str(num))
			num = num + 1
			print("Source Address: " + connection[0].IP_Header.src_ip)
			print("Destination Address " + connection[0].IP_Header.dst_ip)
			print("Source Port: " + str(connection[0].TCP_Header.src_port))
			print("Destination Port: " + str(connection[0].TCP_Header.dst_port))
			print("Status: S" + str(flags[2]) + "F" + str(flags[3]))
			if(flags[1] > 0):
				resetcons = resetcons + 1
			if(flags[2] >0 and flags[3] ==0):
				incompletecons = incompletecons + 1
			if(flags[2] > 0 and flags[3] > 0):
				print("Start Time: " + str(connection[0].timestamp))
				print("End Time: " + str(connection[len(connection)-1].timestamp))
				duration.append(connection[len(connection)-1].timestamp - connection[0].timestamp)
				Mduration = Mduration + connection[len(connection)-1].timestamp - connection[0].timestamp
				completecons = completecons + 1
				stdp = 0
				stdb = 0
				tb = 0
				for i in range(0,len(connection)-1):
					
					if(flags[2] > 0 and flags[3] > 0):
						numcompletepackets = numcompletepackets + 1
						window.append(connection[i].TCP_Header.window_size)
						Mwindow = Mwindow + connection[i].TCP_Header.window_size
						connection[i].get_RTT_value(connection[i+1])
						rtt.append(connection[i].RTT_value)
						Mrtt = Mrtt + (connection[i].RTT_value)
						
					if(connection[i].TCP_Header.src_port == connection[0].TCP_Header.src_port):
						bytes = connection[i].IP_Header.ip_header_len + connection[i].TCP_Header.data_offset
							
						stdp = stdp + 1
						stdb = stdb + connection[i].IP_Header.total_len - bytes
					tb = tb + connection[i].IP_Header.total_len - bytes
				dtsp = len(connection) - stdp
	
				print("Number of Packets sent from Source to Destination: " + str(stdp))
				print("Number of Packets sent from Destination to Source: " + str(dtsp))
				print("Total Number of packets: " + str(len(connection)))
				packets.append(len(connection))
				Mpackets = Mpackets + len(connection)
				print("Number of data bytes sent from Source to Destination: " + str(stdb))
				print("Number of data bytes sent from Destination to Source: " + str(tb - stdb))
				print("Total number of data bytes: " + str(tb))
			print()
	
	print ("C) General")
	print("Total number of complete TCP coneections: " + str(completecons))
	print("Number of reset TCP coneections: " +str(resetcons))
	print("Number of TCP connections that were still open when the trace capture ended: " + str(incompletecons))
	print("_______________________________________________________________________________")
	print()
	
	print("D) Complete TCP connections:")
	print()
	print("Minimum time duration: {:.10f}".format(min(duration)))
	print("Mean time duration: {:.10f}".format((Mduration / completecons)))
	print("Maximum time duration: {:.10f}".format(max(duration)))
	print()
	print("Minimum RTT value: {:.10f}".format(min(rtt)))
	print("Mean RTT value: {:.10f}".format(Mrtt / numcompletepackets))
	print("Maximum RTT value: {:.10f}".format(max(rtt)))
	print()
	print("Minimum number of packets including both send/received: " + str(min(packets)))
	print("Mean number of packets including both send/received: " + str(Mpackets / completecons))
	print("Maximum number of packets including both send/received: " + str(max(packets)))
	print()
	print("Minimum receive window size including both send/received: " + str(min(window)))
	print("Mean receive window size including both send/received: " + str(Mwindow / numcompletepackets))
	print("Maximum receive window size including both send/received: " + str(max(window)))
	tcp.close()
	print()
	print("_______________________________________________________________________________")

	
	
	
	
	
	
if __name__ == "__main__":
	main()