#include "Agent.h"
const double pi = 2 * acos(0.0);
const float chance_pub_rep = 1;


int Agent::allowed_dupes(Data dt) {
	// Get the maximum number of allowed duplicates in that region based off distance to the data area
	float to_point = body.center.distance(dt.target_area);
	int max_dupes_in_area = max_dupes+1 - (to_point / steper);

	if (max_dupes_in_area < 0){
		max_dupes_in_area = 0;
	}

	return max_dupes_in_area;
};

Agent::Agent() {
}

Agent::Agent(int aid, float ax, float ay, float afacing) {
	id = aid;
	facing = afacing;

	body = Circle(ax, ay, 0.01);
	conn_area = Circle(ax, ay, 0.2);

	mem = new AgentMemory();
};

bool Agent::step(vector<Agent*> swarm) {

	move(-0.001 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.004 - -0.001))), 0.0002);

	/* For each private memory if none around replicate to every agent around */
	for (int i = 0; i < mem->pri_mem.size(); i++) {
		vector<Packet> collect = message(swarm, Packet(mem->pri_mem[i], 3, id, -1));

		bool allfalse = true;

		for (int j = 0; j < collect.size(); j++) {
			if (collect[j].type == 1) {
				allfalse = false;
				break;
			}
		}

		if (allfalse) {
			message(swarm, Packet(mem->pri_mem[i], 2, id, -1));
		}
	}
    //~~~~~


	/* Pick random item in public mem */
	int rand_pub;
	if (mem->pub_mem.size() > 0) {
		rand_pub = rand() % mem->pub_mem.size();
	}
	else {
		return false;
	}
	//~~~~~


	/* Replicate if required */
	vector<Packet> collect = message(swarm, Packet(mem->pub_mem[rand_pub], 3, id, -1));

	int counter = 0;

	for (int j = 0; j < collect.size(); j++) {
		if (collect[j].type == 1) {
			counter++;
		}
	}

	// No dupes around, random chance to replicate
	if (counter == 0) {
		float rando = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));

		if (rando <= chance_pub_rep) {
			message(swarm, Packet(mem->pub_mem[rand_pub], 2, id, -1));
		}
	}
	
	// If dupes, do we need more duplicates
	else if (counter < allowed_dupes(mem->pub_mem[rand_pub])) {
		message(swarm, Packet(mem->pub_mem[rand_pub], 2, id, -1));
	}
	//~~~~~








	return true;

	//// Replicate private memory item
	//if (mem->get_pri_index_rep_more_than() != -1){
	//	// Replicate
	//	vector<Packet> collect = message(swarm, Packet(mem->pri_mem[mem->get_pri_index_rep_more_than()], 2, id, -1));

	//	bool all_failed = true;

	//	for (int i = 0; i < collect.size(); i++) {
	//		if (collect[0].type != 0) {
	//			all_failed = false;
	//			mem->pri_mem[mem->get_pri_index_rep_more_than()].replication_num--;
	//			break;
	//		}
	//	}
	//}

	//// Replicate public memory item
	//if (mem->get_pub_index_rep_more_than() != -1) {
	//	//Replicate
	//	vector<Packet> collect = message(swarm, Packet(mem->pub_mem[mem->get_pub_index_rep_more_than()], 2, id, -1));

	//	bool all_failed = true;

	//	for (int i = 0; i < collect.size(); i++) {
	//		if (collect[0].type != 0) {
	//			all_failed = false;
	//			mem->pub_mem[mem->get_pub_index_rep_more_than()].replication_num--;
	//			break;
	//		}
	//	}
	//}

	//
	//// Suicide check random item in pub mem
	//if (mem->pub_mem.size() > 0) {
	//	int ind_d = rand() % mem->pub_mem.size();

	//	// Count how many repliceres around
	//	vector<Packet> collect = message(swarm, Packet(mem->pub_mem[ind_d], 3, id, -1));

	//	int counter = 0;

	//	for (int j = 0; j < collect.size(); j++) {
	//		if (collect[0].type != 0) {
	//			counter++;
	//		}
	//	}

	//	// Get the maximum number of allowed duplicates in that region based off distance to the data area
	//	float to_point = body.center.distance(mem->pub_mem[ind_d].target_area);
	//	int max_dupes_in_area = max_dupes+1 - (to_point / steper);

	//	if (max_dupes_in_area < 0){
	//		max_dupes_in_area = 0;
	//	}

	//	// If above threshold then suicide
	//	if (max_dupes_in_area < counter) {
	//		mem->remove_pub(ind_d);
	//		conns.push_back(pair<int, pair<Coord, Coord>>(4, pair<Coord, Coord>(body.center, Coord(0, 0))));
	//	}
	//}

	//// Chance to increase rep number
	//float rando = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));

	//if (mem->get_pub_index_rep_more_than() == -1 && rando <= chance_pub_rep && mem->pub_mem.size() != 0) {
	//	mem->pub_mem[rand() % mem->pub_mem.size()].replication_num++;
	//}

	//if (mem->get_pri_index_rep_more_than() == -1 && rando <= chance_pri_rep && mem->pri_mem.size() != 0) {
	//	mem->pri_mem[rand() % mem->pri_mem.size()].replication_num++;
	//}

	//// TODO: Here

	//// Sucide complete heuristic stuff

	//// If above threshold then do

	//// Optimise
}

void Agent::move(float dangle, float speed) {
	facing += dangle;

	if (facing > 2 * 2*pi) {
		facing - 2 * pi;
	}
	else if (facing < 0) {
		facing + 2 * pi;
	}

	float dx = cos(facing) * speed;
	float dy = sin(facing) * speed;

	body.translate(dx, dy);
	conn_area.translate(dx, dy);
}

Packet Agent::recieved(Packet packet) {
	if (packet.senderid != id && (packet.recieverid == id || packet.recieverid == -1)) {

		if (packet.type == 1) {
			return Packet(1, id, packet.senderid);
		}

		if (packet.type == 2 && mem->space_in_pub() && packet.data.creator_id != id && !mem->pub_has_data_id(packet.data.id)) {
			mem->push_pub_mem(packet.data);
			return Packet(1, id, packet.senderid);
		}

		if (packet.type == 3 && mem->pub_has_data_id(packet.data.id)) {
			return Packet(1, id, packet.senderid);
		}

	}
	return Packet(0, id, packet.senderid);
}

vector<Packet> Agent::message(vector<Agent*> swarm, Packet packet) {
	vector<Packet> collect;

	for (int i = 0; i < swarm.size(); i++) {
		if (swarm[i]->id != id && conn_area.point_in_circle(swarm[i]->body.center)) {
			Packet temp = swarm[i]->recieved(packet);

			if (temp.type != 0) {
				collect.push_back(temp);
			}

			if (packet.type == 2 && temp.type == 1) {
				conns.push_back(pair<int, pair<Coord, Coord>>(2, pair<Coord, Coord>(body.center, swarm[i]->body.center)));
			}
			else if (packet.type == 2 && temp.type == 0) {
				conns.push_back(pair<int, pair<Coord, Coord>>(3, pair<Coord, Coord>(body.center, swarm[i]->body.center)));
			}

		}
	}

	return collect;
}

std::string Agent::to_string() {
	string line = "Agent " + std::to_string(id) + " X = " + std::to_string(body.center.x) + " Y = " + std::to_string(body.center.y) + "\n";
	line += mem->to_string();
	return line;
}