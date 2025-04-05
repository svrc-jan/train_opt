#!./venv/bin/python3
import sys
import json

from dataclasses import dataclass, field
from functools import cached_property

DEFAULT_INSTANCE = '../data/test/example_problem.json'

@dataclass
class Obj:
	threshold: int 	= field(default=0)
	coeff: int		= field(default=0)
	increment: int	= field(default=0)

@dataclass
class Op:
	train_idx: int		= field(default=-1)
	op_idx: int			= field(default=-1)
	fork_idx: int|None 	= field(default=None)
	
	dur: int			= field(default=0)
	start_lb: int		= field(default=0)
	start_ub: int|None	= field(default=None)
	
	succ: list[int]		= field(default_factory=list)
	res: list[int]		= field(default_factory=list)
	res_time: list[int]	= field(default_factory=list)

	obj: Obj|None		= field(default=None)

	@cached_property
	def n_succ(self):
		return len(self.succ)
	
	@cached_property
	def n_res(self):
		return len(self.res)
	
	@cached_property
	def idx(self):
		return (self.train_idx, self.op_idx)
	
	@cached_property
	def name(self):
		return f'({self.train_idx},{self.op_idx})'

class Instance:
	ops : list[list[Op]]
	forks : list[Op]
	res_idx : dict[str, int]
	total_dur : int

	def __init__(self, file_name):
		self.parse_json(file_name)

	def parse_json(self, file_name):
		self.ops = []
		self.res_idx = {}
		self.total_dur = 0

		with open(file_name, 'r') as fp:
			jsn = json.load(fp)

		for train_idx, train_jsn in enumerate(jsn['trains']):
			train_ops = []
			for op_idx, op_jsn in enumerate(train_jsn):
				op = Op(
					train_idx	= train_idx,
					op_idx	 	= op_idx,
					dur			= op_jsn['min_duration'],
					start_lb	= op_jsn.get('start_lb', 0),
					start_ub	= op_jsn.get('start_ub', None)
				)

				self.total_dur += op.dur

				for succ in op_jsn['successors']:
					op.succ.append(succ)

				if 'resources' in op_jsn:
					for res_jsn in op_jsn['resources']:
						res_name = res_jsn['resource']
						res_time = res_jsn.get('release_time', 0)
						
						if res_name in self.res_idx:
							res_idx = self.res_idx[res_name]
						else:
							res_idx = len(self.res_idx)
							self.res_idx[res_name] = res_idx

						op.res.append(res_idx)
						op.res_time.append(res_time)

					
				train_ops.append(op)

			self.ops.append(train_ops)

		self.n_ops = [len(t) for t in self.ops]
		self.start_ops = [t[0] for t in self.ops]
		
		for obj_jsn in jsn['objective']:
			if obj_jsn['type'] != 'op_delay':
				continue

			train_idx = obj_jsn['train']
			op_idx = obj_jsn['operation']
			
			obj = Obj()
			obj.threshold = obj_jsn.get('threshold', 0)
			obj.coeff 	  = obj_jsn.get('coeff', 0)
			obj.increment = obj_jsn.get('increment', 0)

			assert(obj.coeff == 0 or obj.increment == 0)

			self.ops[train_idx][op_idx].obj = obj

		self.forks = []
		for train in self.ops:
			for op in train:
				if op.n_succ > 1:
					op.fork_idx = len(self.forks)
					self.forks.append(op)


	def get_swaps(self):
		pass

	@cached_property
	def n_trains(self):
		return len(self.ops)
	
	@cached_property
	def n_res(self):
		return len(self.res_idx)
	
	@cached_property
	def n_forks(self):
		return len(self.forks)

if __name__ == '__main__':
	inst = Instance(sys.argv[1] if len(sys.argv) > 1 else DEFAULT_INSTANCE)
	print(inst.forks)


