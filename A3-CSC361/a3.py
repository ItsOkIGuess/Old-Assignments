import sys
import struct
import basic_structures as bs

def sortfunc(x):
	return x[1]

def checkConnection(tup,cons):
	connection = []
	for i in range(0,len(cons) - 1):
		
		if(tup[0] == cons[i].IP_Header.src_ip and tup[1] == cons[i].IP_Header.dst_ip):
			connection.append(cons[i])

		if(tup[1] == cons[i].IP_Header.src_ip and tup[0] == cons[i].IP_Header.dst_ip):
			connection.append(cons[i])

	count = 0
	j = 1
	sumRTT = 0
	for i in range(0,len(connection) - 2):
		sumRTT = connection[j].timestamp - connection[i].timestamp
		j = j + 1
		count = count + 1
	
	if(count != 0):
		print(tup[0] + " " + str(sumRTT/count))
	
	return connection


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
	TCP = False
	UDP = False
	ICMP = False
	fragment = []
	
	while(tcp):
		pack = bs.packet()
		list.append(pack)
		data = tcp.read(16)
		
		if len(data) == 0:
			break
		list[x].packet_No_set(x)
		list[x].timestamp_set(data[0:4],data[4:8],timestart)
		timestart = list[0].timestamp
		size = struct.unpack("<ll", data[8:16])
		data = tcp.read(size[0])
		
		h = bs.IP_Header()
		h.get_header_len(data[14:15])
		h.get_total_len(data[16:18])
		h.get_IP(data[(6+h.ip_header_len):(10+h.ip_header_len)],data[(10+h.ip_header_len):(14+h.ip_header_len)])
		h.set_protocol(data[23:24])
		if(h.protocol == 1 or h.protocol == 17):
			h.set_ttl(data[50:51])
			boop = h.get_frag(data[20:22])
			sloop = struct.unpack('H',data[18:20])[0]
			coop = (boop,sloop)
			fragment.append(coop)

		list[x].IP_Header = h
		x = x + 1
		
	list[0].timestamp = 0.000000

	cons = []
	tups = []
	num = 1
	flags = []
	

	
	for i in range(0,len(list)-1):
		if(list[i].IP_Header.protocol == 6):
			TCP = True
		if(list[i].IP_Header.protocol == 1):
			ICMP = True
		if(list[i].IP_Header.protocol == 17):
			UDP = True
		if(list[i].IP_Header.protocol == 1):
			t = (list[i].IP_Header.src_ip, list[i].IP_Header.dst_ip)
			tt = (list[i].IP_Header.dst_ip, list[i].IP_Header.src_ip)
			if(t not in tups and tt not in tups):
				tups.append(t)
				tups.append(tt)
				cons.append(checkConnection(t,list))


	j = 0
	while(cons[j][0].IP_Header.protocol != 17):
		j = j + 1
		if j == len(cons)-1:
			break
	print("The IP address of the source node: " + cons[j][0].IP_Header.src_ip)
	print("The IP address of the ultimate destination node: " + cons[j][0].IP_Header.dst_ip)	
	Snode = cons[j][0].IP_Header.src_ip
	Dnode = cons[j][0].IP_Header.dst_ip
	print()
	
	ips = []
	uniqueIPS = []
	
	for element in cons:
		tup = element[0].IP_Header.src_ip, element[0].IP_Header.ttl
		ips.append(tup)
		tup = element[0].IP_Header.dst_ip, element[0].IP_Header.ttl
		ips.append(tup)
		
	ips.sort(key=sortfunc)

	for i in range(0,len(ips)):
		x = ips[i]
		#print(x)
		if(x[0] not in uniqueIPS):
			uniqueIPS.append(x[0])
	count = 1
	#uniqueIPS.sort(key=sortfunc)
	for element in uniqueIPS:
		if(element != Snode and element != Dnode):
			print("router " + str(count) + ": " + element)
			count = count + 1
	print()
	print("The values in the protocol field of IP headers:")
	if(ICMP):
		print("1: ICMP")
			
	if(UDP):
		print("17: UDP")
	
	
	print()
	IAMCOUNTING = 0
	packIds = []
	numfrags = []
	maxID = 0
	for element in fragment:
		if(element[0] not in numfrags):
			numfrags.append(element[0])
		if(element[0] > 0 and element[1] not in packIds):
			packIds.append(element)
			IAMCOUNTING = IAMCOUNTING + 1
	if(IAMCOUNTING != 0):
		print("The number of fragments created from the original datagram is: " + str(len(numfrags)))
		print("The offset of the last fragment is: " + str(packIds[len(packIds) - 1][0]))





















if __name__ == "__main__":
	main()