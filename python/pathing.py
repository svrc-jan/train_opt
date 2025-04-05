#!./venv/bin/python3

import sys
import random

from pyscipopt import Model, quicksum

from instance import Instance, Op

DEFAULT_INSTANCE = '../data/test/headway1.json'

class Pathing:
	inst: Instance
	fork_select: list[int] 
	fork_active: list[bool]
	paths: list[list[Op]]
	res_ops: dict[int, list[tuple[Op, Op, int]]]

	def __init__(self, inst):
		self.inst = inst
	
	def make_random_fork_select(self):
		self.fork_select = [random.choice(op.succ) for op in self.inst.forks]

	def make_paths(self):
		self.fork_active = [False]*self.inst.n_forks

		self.paths = []
		for train_idx in range(self.inst.n_trains):
			train_ops = self.inst.ops[train_idx]
			train_path = [train_ops[0]]

			while train_path[-1].n_succ > 0:
				last_op = train_path[-1]

				if last_op.n_succ > 1: # fork
					self.fork_active[last_op.fork_idx] = True
					train_path.append(train_ops[self.fork_select[last_op.fork_idx]])
				else:
					train_path.append(train_ops[last_op.succ[0]])

			self.paths.append(train_path)

	def make_res_ops(self):
		self.res_ops = {}
		for path in self.paths:
			for op, next in zip(path, path[1:] + [None]):
				for res, time in zip(op.res, op.res_time):
					if not res in self.res_ops:
						self.res_ops[res] = []

					self.res_ops[res].append((op, next, time))

	def create_ilp_model(self):
		m = Model()
		
		omega = self.inst.total_dur
		eps = 0

		x = {} # starts of ops
		o = {} # ordering
		z = {}
		z_c = {}

		for path in self.paths:
			for op in path:
				x[op.idx] = m.addVar(name=f'x{op.name}', 
					vtype='C', lb=op.start_lb, ub=op.start_ub)
				o[op.idx] = m.addVar(name=f'o{op.name}', 
					vtype='C', lb=0, ub=None)

				if op.obj is not None:
					if op.obj.coeff > 0:
						name = f'z{op.name}'
						
						z_c[op.idx] = op.obj.coeff
						z[op.idx] = m.addVar(name=name, vtype='C', lb=0, ub=None)
						
						m.addCons(z[op.idx] >= x[op.idx] - op.obj.threshold)

					
					elif op.obj.increment > 0:
						name = f'zb{op.train_idx},{op.op_idx}'
						
						z_c[op.idx] = op.obj.increment
						z[op.idx] = m.addVar(name=name, vtype='B')
						
						m.addCons(omega*z[op.idx] >= x[op.idx] - op.obj.threshold)

		m.setObjective(quicksum(z_c[key]*z[key] for key in z.keys()), 'minimize')

		for path in self.paths:
			for prev, next in zip(path[:-1], path[1:]):
				m.addCons(x[prev.idx] + prev.dur <= x[next.idx])


		
		y = {} # binary if 
		for res, ops in self.res_ops.items():
			for op1, next1, time1 in ops:
				for op2, next2, time2 in ops:
					if op1 == op2:
						continue
					
					assert(next1 is not None or next2 is not None)

					if next1 is None:
						m.addCons(x[op1.idx] >= x[next2.idx])

					elif next2 is None:
						m.addCons(x[op2.idx] >= x[next1.idx])


					name = f'y{res},{op1.name},{op2.name}'
					y1 = m.addVar(name=name, vtype='B')
					y[(res, op1.idx, op2.idx)] = y1

					name = f'y{res},{op2.name},{op1.name}'
					y2 = m.addVar(name=name, vtype='B')
					y[(res, op2.idx, op1.idx)] = y2

					m.addCons(y1 + y2 == 1)
					m.addCons(x[op2.idx] + omega*(1-y1) >= x[next1.idx] + time1)
					m.addCons(x[op1.idx] + omega*(1-y2) >= x[next2.idx] + time2)


					
					# m.addCons(o[op2.idx] + omega*(1-y1) >= o[next1.idx] + 1)
					# m.addCons(o[op1.idx] + omega*(1-y2) >= o[next2.idx] + 1)
					

		return m, (x, y, z)

	def create_plan(self):
		pass 


if __name__ == '__main__':
	inst = Instance(sys.argv[1] if len(sys.argv) > 1 else DEFAULT_INSTANCE)
	pth = Pathing(inst)

	pth.make_random_fork_select()
	pth.make_paths()
	pth.make_res_ops()

	m, vars = pth.create_ilp_model()
	x, y, z = vars
	m.optimize()

	# if (m.getStatus() == 'optimal'):
	# 	for idx, var in x.items():
	# 		print(idx, m.getVal(var))

		# for idx, var in y.items():
		# 	print(idx, m.getVal(var))