#!/usr/bin/env python3

import argparse
import random
import string

class MakefileGenerator:
    def __init__(self):
        args = self.parse_args()

        self.args = args
        self.output_file = args['output_file']
        self.num_connections = args['num_connections']
        self.wait_time = args['wait_time']
        self.num_nodes = args['num_nodes']
        self.graph = self.generate_graph()

    def __del__(self):
        self.output_file.close()

    def parse_args(self):
        parser = argparse.ArgumentParser(description='Generate a test makefile!')
        
        parser.add_argument('-o', '--output_file', type=argparse.FileType('w'), required=True, help='name of the Makefile to generate')
        parser.add_argument('-c', '--num_connections',  type=int, help='number of connections for each node to have', default=3)
        parser.add_argument('-w', '--wait_time', type=float, help='maximum amount of time to sleep for at each node', default=1.0)
        parser.add_argument('-n', '--num_nodes', type=int, help='number of nodes to generate in the dependency graph', default=50)

        return vars(parser.parse_args())

    def generate_node_name(self, size=80):
        return ''.join([random.choice(string.ascii_uppercase + string.digits) for _ in range(size)])

    def generate_graph(self):
        graph = {}
        nodes = [self.generate_node_name() for _ in range(self.num_nodes)]
        
        for node in nodes:
            graph[node] = set()

        for i in range(self.num_nodes - self.num_connections):
            node = nodes[i]
            for _ in range(self.num_connections):
                graph[node].add(nodes[random.randint(i + 1, self.num_nodes - 1)])
        
        return graph

    def generate_makefile(self):
        dependents = set()
        for dependencies in self.graph.values():
            for dependency in dependencies:
                dependents.add(dependency)
        
        roots = set(self.graph.keys()) - dependents
        self.graph['all'] = roots

        for node, dependencies in self.graph.items():
            self.output_file.write('{}: {}\n'.format(node, ' '.join(dependencies)))
            self.output_file.write('\techo "Starting node {}"\n'.format(node))
            self.output_file.write('\tsleep {}\n'.format(random.random() * self.wait_time))
            self.output_file.write('\techo "Finished node {}"\n'.format(node))
            self.output_file.write('\n',)
            
def main():
    gen = MakefileGenerator()
    gen.generate_makefile()


if __name__ == '__main__':
    main()
