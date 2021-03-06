#!/usr/bin/env python

import rospy
from geometry_msgs.msg import Point
from ogm_communications.srv import ServerRequestStructuralEvaluation
from ogm_communications.srv import ServerRequestStructuralEvaluationResponse
from graph_matching import GraphMatching


class StructuralEvaluation:

    # Constructor
    def __init__(self):

        print "Created StructuralEvaluation instance"
        structuralEval = rospy.Service('/structural_evaluation/map_evaluation',
                                       ServerRequestStructuralEvaluation,
                                       self.callback)

    def callback(self, req):
        parameters = {}
        parameters['skeletonizationMethod'] = req.skeletonizationMethod
        parameters['pruning'] = req.pruning
        parameters['morphOpen'] = req.morphOpen
        parameters['pruningIterations'] = int(req.pruningIterations)
        parameters['morphOpenIterations'] = int(req.morphOpenIterations)
        parameters['gaussianBlur1'] = req.gaussianBlur1
        parameters['gaussianBlur2'] = req.gaussianBlur2
        parameters['medianBlur1'] = req.medianBlur1
        parameters['medianBlur2'] = req.medianBlur2
        parameters['gaussianKernel1'] = int(req.gaussianKernel1)
        parameters['gaussianKernel2'] = int(req.gaussianKernel2)
        parameters['medianKernel1'] = int(req.medianKernel1)
        parameters['medianKernel2'] = int(req.medianKernel2)

        res = ServerRequestStructuralEvaluationResponse()

        # Create graph matching instance
        self.graphMatching = GraphMatching(req.map1, req.map2)
        
        res.voronoi1 = []
        res.vertices1 = []
        res.vertices2 = []
        res.voronoi2 = []
        # Extract the TopologicalGraphs
        voronoi1, vertices1, voronoi2, vertices2 = self.graphMatching.extractTopologicalGraphs(parameters)
        for v in voronoi1:
            p = Point()
            p.x = v[0]
            p.y = v[1]
            res.voronoi1.append(p)
        
        for v in voronoi2:
            p = Point()
            p.x = v[0]
            p.y = v[1]
            res.voronoi2.append(p)
      
        for v in vertices1:
            p = Point()
            p.x = v[0]
            p.y = v[1]
            res.vertices1.append(p)
      
        for v in vertices2:
            p = Point()
            p.x = v[0]
            p.y = v[1]
            res.vertices2.append(p)

        return res


