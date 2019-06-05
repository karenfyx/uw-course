#!/usr/bin/env python3 
import sys
import math
from datetime import datetime
from socket import *
from struct import *
LOGFILE = ''


def init_lsdb(circuit_db, id):
	for link in circuit_db.linkcost:
		ls = Link(id,link.link_id,link.cost)
		source.adj_lst.append(ls)
		unmatched[link.link_id] = ls
	log_lsdb()

def log_pkt(pkt,action):
	with open(LOGFILE, "a") as f:
		f.write('#'+pkt.msg(action))

def log_lsdb():
	with open(LOGFILE, "a") as f:
		f.write('#Topology database \n')
		for router in lsdb :
			for link in router.adj_lst:
				dest = None
				if link.dest:
					dest = link.dest.router_id
				f.write('R'+str(router.router_id)+' link_id '+str(link.link_id)+' cost '+str(link.cost)+'\n')

def log_rib():
	with open(LOGFILE, "a") as f:
		f.write('#RIB\n')
		for router in lsdb :
			if router == source :
				f.write('R'+str(id)+' -> R'+str(id)+' Local, 0\n')
			elif router.distance == math.inf :
				f.write('R'+str(id)+' -> R'+str(router.router_id)+' Unreachable, INF\n')
			else :
				f.write('R'+str(id)+' -> R'+str(router.router_id)+' '+str(router.next_hop.router_id)+' '+str(router.distance)+'\n')

def dijsktra():
	#Initialization
	for node in lsdb:
		node.distance = math.inf
	source.distance = 0
	for link in source.adj_lst:
		if link.dest:
			link.dest.distance = link.cost
			link.dest.prev = source
	visited = set()
	visited.add(source)
	while 1:
		#Find node with minimal distance: min_node
		min_node = None
		for node in lsdb:
			if node in visited or node.distance == math.inf :
				pass
			else :
				if not min_node or node.distance < min_node.distance :
					min_node = node
		if not min_node: break
		visited.add(min_node)
		#Update neighbour of min_node
		for link in min_node.adj_lst:
			if link.dest and not link.dest in visited:
				if link.dest.distance > link.cost+min_node.distance :
					link.dest.distance = link.cost+min_node.distance
					link.dest.prev = min_node
		#Backtrace to find the next_hop value of min_node
		node = min_node
		while not node.prev == source:
			node = node.prev
		min_node.next_hop = node
	log_rib()


def main():
	#INIT(ASSUME CORRECT INPUT)
	id = int(sys.argv[1])
	nse_host = sys.argv[2]
	nse_port, router_port = int(sys.argv[3]), int(sys.argv[4])
	LOGFILE = 'router' + sys.argv[1] + '.log'
	unmatched = {}  # To match both sides of a link, record unmached link
	lsdb = [Node(i + 1) for i in range(NBR_ROUTER)]
	source = lsdb[id - 1]

	with open(LOGFILE, "w") as f:
		f.write('***Router'+sys.argv[1]+' log, '+str(datetime.now())+'***\n')

	#SEND INIT
	udp_socket = socket(AF_INET, SOCK_DGRAM)
	udp_socket.bind(('',router_port))
	init_pkt = pkt_INIT(id)
	udp_socket.sendto(init_pkt.toByte(),(nse_host, nse_port))
	log_pkt(init_pkt,'Send')
	#RECEIVE circuit_db
	buffer, nse_addr = udp_socket.recvfrom(2048)
	circuit_db = circuit_DB(buffer)
	log_pkt(circuit_db,'Receive')
	init_lsdb(circuit_db, id)

	#SEND HELLO PKT TO NEIGHBOURS
	for link in circuit_db.linkcost:
		hello_pkt = pkt_HELLO(id,link.link_id)
		udp_socket.sendto(hello_pkt.toByte(),(nse_host, nse_port))
		log_pkt(hello_pkt,'Send')

	while 1:
		buffer, nse_addr = udp_socket.recvfrom(2048)
		msg_len = len(buffer)
		if msg_len == 8: #handle pkt_HELLO
			sender_id, via = parseUDPdata(buffer)
			log_pkt(pkt_HELLO(sender_id,via),'Receive')
			for link in source.adj_lst :
							if link.link_id == via : #Match link to neighbor
									link.dest = lsdb[sender_id-1]
			#SEND LINK STATE
			for router in lsdb:
				for link in router.adj_lst:
					lspdu = pkt_LSPDU(id,link.router_id,link.link_id,link.cost,via)
					udp_socket.sendto(lspdu.toByte(),(nse_host, nse_port))
					log_pkt(lspdu,'Send')
		elif msg_len == 20: #handle pkt_LSPDU
			lspdu = pkt_LSPDU()
			lspdu.parse(buffer)
			cur_router = lsdb[lspdu.router_id-1]
			if not cur_router.checkDupeLS(lspdu.link_id): #SKIP IF LS IS DUPLICATED
				log_pkt(lspdu,'Receive duplicated')
				continue
			#FORWARD TO NEIGHBORS
			log_pkt(lspdu,'Receive')
			lspdu.sender = id
			for link in source.adj_lst:
				if link.dest and not link.dest.router_id == lspdu.sender: #IF HELLOED AND NOT SENDER
					lspdu.via = link.link_id
					udp_socket.sendto(lspdu.toByte(),(nse_host, nse_port))
					log_pkt(lspdu,'Forward')
			#RECORD NEW LS
			ls = Link(lspdu.router_id,lspdu.link_id,lspdu.cost)
			cur_router.adj_lst.append(ls)
			#Try getting both directions of a link
			match = unmatched.get(lspdu.link_id)
			if not match:
				unmatched[lspdu.link_id] = ls
			else:
				ls.dest = lsdb[match.router_id-1]
				match.dest = cur_router
			log_lsdb()
			dijsktra()

		else :
			print('Unrecognized message. Neither length 8 nor length 20')


if __name__ == "__main__":
	main()