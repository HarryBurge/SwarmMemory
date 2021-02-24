#include "Agent.h"
const double pi = 2 * acos(0.0);
const int facter = 40;

Agent::Agent() {
}

Agent::Agent(int aid, float ax, float ay, float afacing) {
	id = aid;
	facing = afacing;
	iterator = 0;
	lastactions = 0;

	stability = 0;

	body = Circle(ax, ay, 0.01);
	conn_area = Circle(ax, ay, 0.25);

	mem = new AgentMemory();
};

void Agent::step(vector<Agent*> swarm) {

	/* Movement keeping away from other agents but need to stay near somewhere */
	//move(-0.001 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.004 - -0.001))), 0.0002);

	vector<Vec> localagents;

	for (int i = 0; i < swarm.size(); i++) {
		if (swarm[i]->id != id && conn_area.point_in_circle(swarm[i]->body.center)) {
			float dist = body.center.distance(swarm[i]->body.center);
			float invdist = 0.24 - dist;

			Vec originalvec = Vec(body.center.x - swarm[i]->body.center.x, body.center.y - swarm[i]->body.center.y);
			localagents.push_back(originalvec.set_magnitude(invdist));
		}
	}

	Vec point_to_look = Vec();

	for (int i = 0; i < localagents.size(); i++) {
		point_to_look = point_to_look.add(localagents[i]);
	}

	// keep roughly towards the center
	point_to_look = point_to_look.add(Vec(-body.center.x, -body.center.y).set_magnitude( (sqrt(8) - body.center.distance(Coord(0,0)))*0.03 ));

	float to = facing;

	if (isnan(point_to_look.angle_origin())) {

	}
	else if (point_to_look.a < 0){ // can switch around and it will move away
		to = point_to_look.angle_origin() + pi;
	}
	else if (point_to_look.a > 0) {
		to = point_to_look.angle_origin();
	}

	facing = to;

	/*if (to-0.003 < facing < to+0.003) {
		facing = to;
	}
	else if (facing < to) {
		facing += 0.003;
	}
	else if (facing > to) {
		facing -= 0.003;
	}*/

	move(0, 0.0002);
	


	/* Private memory replicate if no duplicates around */
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


	/* Go through algorithm for item at iterator */
	if (mem->pub_mem.size() > 0) {
		
		/* Count dupes and get ratio */
		vector<Packet> collect1 = message(swarm, Packet(mem->pub_mem[iterator], 3, id, -1));

		float current_dupes = 0;

		for (int j = 0; j < collect1.size(); j++) {
			if (collect1[j].type == 1) {
				current_dupes++;
			}
		}

		float dupes_ratio = 0;
		
		if (collect1.size() != 0) {
			dupes_ratio = current_dupes / collect1.size();
		}


		/* Distance to point bounded */
		float to_point = body.center.distance(mem->pub_mem[iterator].target_area);


		/* Available public memory in agents around averaged */
		float average_public_spare = 0;

		vector<Packet> collect2 = message(swarm, Packet(mem->pub_mem[iterator], 4, id, -1));

		float total_num = 0;

		for (int j = 0; j < collect2.size(); j++) {

			if (collect2[j].type == 1) {
				average_public_spare += (float) collect2[j].additional;
				total_num++;
			}

		}

		if (total_num != 0) {
			average_public_spare = average_public_spare / total_num;
		}

		average_public_spare = average_public_spare / pub_max_size;


		float alphar = 20;
		float betar = 2;
		float dynweightr = 0.6;


		float b1 = 0.45;
		float b2 = 0.45;
		float b3 = 0.1;

		float heuristic_rep = b1 * (1 - dupes_ratio) + b2 * ((2.82843 - to_point) / 2.82843) + b3 * average_public_spare -dynweightr * ((1 / (1 + exp(-((float)stability - alphar) / betar))));
		
		if (heuristic_rep > 0.9) {
			message(swarm, Packet(mem->pub_mem[iterator], 2, id, -1));
		}

		float alphas = 150;
		float betas = 2;
		float dynweights = 0.6;

		float p1 = 0.3;
		float p2 = 0.7;

		float heuristic_sui = p1 * dupes_ratio + p2 * (to_point / 2.82843) -dynweights * (-(1 / (1 + exp(-((float)lastactions - alphas) / betas))) + 1);

		if (heuristic_sui > 0.325) {
			mem->remove_pub(iterator);
			lastactions = 0;
		}

		lastactions++;

		stability--;
		if (stability < 0) {
			stability = 0;
		}
	}


	/* Update iterater */
	iterator++;

	if (iterator >= mem->pub_mem.size()) {
		iterator = 0;
	}

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

		if (packet.type == 2 && packet.data.creator_id != id && mem->space_in_pub() && !mem->pub_has_data_id(packet.data.id) && !mem->pri_has_data_id(packet.data.id)) {
			stability += facter;
			mem->push_pub_mem(packet.data);
			return Packet(1, id, packet.senderid);
		}

		if (packet.type == 3 && mem->pub_has_data_id(packet.data.id)) {
			return Packet(1, id, packet.senderid);
		}

		if (packet.type == 4 && !mem->pub_has_data_id(packet.data.id)) {
			return Packet(1, id, packet.senderid, pub_max_size - mem->pub_mem.size());
		}

	}
	return Packet(0, id, packet.senderid);
}

vector<Packet> Agent::message(vector<Agent*> swarm, Packet packet) {
	vector<Packet> collect;

	for (int i = 0; i < swarm.size(); i++) {
		if (swarm[i]->id != id && conn_area.point_in_circle(swarm[i]->body.center)) {
			Packet temp = swarm[i]->recieved(packet);

			//if (temp.type != 0) {
			collect.push_back(temp);
			//}

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