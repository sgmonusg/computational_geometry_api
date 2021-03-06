//
//  DCEL.cpp
//  ComputationalGeometryFinal
//
//  Created by Shikhar  on 19/04/17.
//  Copyright © 2017 Shikhar . All rights reserved.
//

#include "DCEL.hpp"
DCEL::DCEL(Data ob) //CHECKED
{
    int N=ob.getSize();
    if(N<=2)
    {
        cout<<"Can't form a DCEL on less than 3 points\n"; //raise exception here.
        return;
    }
    for(int i=0;i<N;i++)
    {
        Vertex t(i);
        VertexRecord[t.myaddr]=(t);
    }
    for(int i=0;i<2*N;i++)
    {
        Edge t(i);
        EdgeRecord[t.myaddr]=(t);
    }
    Face f1(0),f2(1);
    FaceRecord[f1.myaddr]=(f1);
    FaceRecord[f2.myaddr]=(f2);
    FaceRecord[0].p_incident=&EdgeRecord[0];
    FaceRecord[1].p_incident=&EdgeRecord[N];
    for(int i=0;i<N;i++)
    {
        VertexRecord[i].coord=ob.getPoint(i);
        VertexRecord[i].p_incident=&EdgeRecord[i];
        EdgeRecord[i].p_start=&VertexRecord[i];
        EdgeRecord[i].p_twin=&EdgeRecord[i+N];
        EdgeRecord[i].p_next=&EdgeRecord[(i+1)%N];
        EdgeRecord[i].p_prev=&EdgeRecord[(i-1)>=0?(i-1):(N-1)];
        EdgeRecord[i].p_incident=&FaceRecord[0];
        
    }
    int c=1;
    for(int i=N;i<2*N;i++)
    {
        EdgeRecord[i].p_twin=&EdgeRecord[i-N];
        EdgeRecord[i].p_next=&EdgeRecord[((i)==N)?2*N-1:i-1];
        EdgeRecord[i].p_prev=&EdgeRecord[((i+1)==2*N)?N:(i+1)];
        EdgeRecord[i].p_start=&VertexRecord[(c++)%N];
        EdgeRecord[i].p_incident=&FaceRecord[1];
    }
    
}
void DCEL::addEdge(Vertex start,Vertex end)
{
    cout<<"NOW ADDING EDGE BETWEEN "<<start.myaddr<<" "<<end.myaddr<<endl;
    //check for the case when start and end are already connected by a single edge return without doing anything .
    //generate face lists from both end and find common face to split add edge update prev next
    vector<Face> list_start=incidentFaces(start);
    vector<Face> list_end=incidentFaces(end);
    Face splitFace;
    bool flag=false;
    for(vector<Face>::iterator it=list_start.begin();it!=list_start.end();it++)
    {
        for(vector<Face>::iterator it2=list_end.begin();it2!=list_end.end();it2++)
            if(*it==*it2 && it->myaddr!=1)
            {
                splitFace=*it;
                flag=true;
                break;
            }
        if(flag)
            break;
    }
    // Face splitFace=FaceRecord[0];
    ////////////
    //cout<<splitFace.myaddr<<endl;
    ////////////
    Edge onBoundary=*(splitFace.p_incident);
    while(*(onBoundary.p_start)!=start)
    {
        onBoundary=*(onBoundary.p_next);
    }
    
    
    Face rightFace((int)FaceRecord.size());
    Face leftFace=splitFace;
    FaceRecord[rightFace.myaddr]=rightFace;
    Edge split_edge((int)EdgeRecord.size()); //add new element to record
    EdgeRecord[split_edge.myaddr]=split_edge;
    Edge split_edge2((int)EdgeRecord.size());
    EdgeRecord[split_edge2.myaddr]=split_edge2;
    
    EdgeRecord[split_edge.myaddr].p_start=&VertexRecord[end.myaddr];
    EdgeRecord[split_edge.myaddr].p_next=&EdgeRecord[onBoundary.myaddr];
    
    
    EdgeRecord[onBoundary.myaddr].p_prev=&(EdgeRecord[split_edge.myaddr]); ////
    Edge prevtoOnBoundary=*(onBoundary.p_prev);
    
    EdgeRecord[split_edge2.myaddr].p_start=&VertexRecord[start.myaddr];
    EdgeRecord[split_edge2.myaddr].p_prev=&EdgeRecord[prevtoOnBoundary.myaddr];
    EdgeRecord[split_edge2.myaddr].p_twin=&EdgeRecord[split_edge.myaddr];
    
    
    EdgeRecord[split_edge.myaddr].p_twin=&EdgeRecord[split_edge2.myaddr];
    EdgeRecord[prevtoOnBoundary.myaddr].p_next=&EdgeRecord[split_edge2.myaddr];
    while(*(onBoundary.p_twin->p_start)!=end)
    {
        onBoundary=*(onBoundary.p_next);
    }
    
    EdgeRecord[split_edge.myaddr].p_prev=&EdgeRecord[onBoundary.myaddr];
    EdgeRecord[split_edge.myaddr].p_incident=&FaceRecord[rightFace.myaddr]; //right face is the new face or top to bottom's (Split edge's) incident face
    EdgeRecord[split_edge2.myaddr].p_next=&EdgeRecord[(onBoundary.p_next->myaddr)];
    EdgeRecord[onBoundary.p_next->myaddr].p_prev=&EdgeRecord[split_edge2.myaddr];
    EdgeRecord[onBoundary.myaddr].p_next=&EdgeRecord[split_edge.myaddr];
    EdgeRecord[split_edge2.myaddr].p_incident=&FaceRecord[leftFace.myaddr];
    FaceRecord[leftFace.myaddr].p_incident=&EdgeRecord[split_edge2.myaddr];
    FaceRecord[rightFace.myaddr].p_incident=&EdgeRecord[split_edge.myaddr];
    
    while(*(onBoundary.p_twin->p_start)!=start)
    {
        //cout<<"Done on --"<<onBoundary.myaddr<<" ";
        EdgeRecord[onBoundary.myaddr].p_incident=&FaceRecord[rightFace.myaddr];
        onBoundary=*(onBoundary.p_prev);
    }
    EdgeRecord[onBoundary.myaddr].p_incident=&FaceRecord[rightFace.myaddr];
    //cout<<"EDGE OF FACE 0 is "<<FaceRecord[0].myaddr<<"endl";
    //cout<<"$$$$$EDGE TABLE$$$$\n";
    //for(unordered_map<int,Edge>::iterator it=EdgeRecord.begin();it!=EdgeRecord.end();it++)
    //{
        
     //   cout<<it->first<<"\t"<<it->second.p_incident->myaddr<<endl;
    //}
    //cout<<"$$$$$EDGE TABLE$$$$\n";
}

void DCEL::addVertex(Edge &e,Point p)
{
    Edge leftlower((int)EdgeRecord.size());
    Edge leftupper((int)EdgeRecord.size()+1);
    Edge rightupper((int)EdgeRecord.size()+2);
    Edge rightlower((int)EdgeRecord.size()+3);
    EdgeRecord[leftlower.myaddr]=leftlower;
    EdgeRecord[leftupper.myaddr]=leftupper;
    EdgeRecord[rightupper.myaddr]=rightupper;
    EdgeRecord[rightlower.myaddr]=rightlower;
    
    Vertex joint((int)VertexRecord.size()); //for puns
    if(*e.p_incident->p_incident==e)
    {
        e.p_incident->p_incident=e.p_next;
    }
    joint.coord=p;
    joint.p_incident=&EdgeRecord[leftupper.myaddr];
    VertexRecord[joint.myaddr]=joint;
    
    EdgeRecord[e.p_prev->myaddr].p_next=&EdgeRecord[leftlower.myaddr];
    EdgeRecord[e.p_next->myaddr].p_prev=&EdgeRecord[leftupper.myaddr];
    EdgeRecord[e.p_twin->p_prev->myaddr].p_next=&EdgeRecord[rightupper.myaddr];
    EdgeRecord[e.p_twin->p_next->myaddr].p_prev=&EdgeRecord[rightlower.myaddr];
    
    
    EdgeRecord[leftupper.myaddr].p_incident=e.p_incident;
    EdgeRecord[leftlower.myaddr].p_incident=e.p_incident;
    EdgeRecord[rightlower.myaddr].p_incident=e.p_twin->p_incident;
    EdgeRecord[rightupper.myaddr].p_incident=e.p_twin->p_incident;
    
    EdgeRecord[leftlower.myaddr].p_next=&EdgeRecord[leftupper.myaddr];
    EdgeRecord[leftupper.myaddr].p_next=e.p_next;
    EdgeRecord[rightlower.myaddr].p_next=e.p_twin->p_next;
    EdgeRecord[rightupper.myaddr].p_next=&EdgeRecord[rightlower.myaddr];
    
    EdgeRecord[leftlower.myaddr].p_prev=e.p_prev;
    EdgeRecord[leftupper.myaddr].p_prev=&EdgeRecord[leftlower.myaddr];
    EdgeRecord[rightlower.myaddr].p_prev=&EdgeRecord[rightupper.myaddr];
    EdgeRecord[rightupper.myaddr].p_prev=e.p_twin->p_prev;
    
    EdgeRecord[leftlower.myaddr].p_twin=&EdgeRecord[rightlower.myaddr];
    EdgeRecord[leftupper.myaddr].p_twin=&EdgeRecord[rightupper.myaddr];
    EdgeRecord[rightlower.myaddr].p_twin=&EdgeRecord[leftlower.myaddr];
    EdgeRecord[rightupper.myaddr].p_twin=&EdgeRecord[leftupper.myaddr];
    
    EdgeRecord[leftlower.myaddr].p_start=e.p_start;
    EdgeRecord[leftupper.myaddr].p_start=&VertexRecord[joint.myaddr];
    EdgeRecord[rightlower.myaddr].p_start=&VertexRecord[joint.myaddr];
    EdgeRecord[rightupper.myaddr].p_start=e.p_twin->p_start;
    
    EdgeRecord.erase(EdgeRecord.find(e.myaddr));
    EdgeRecord.erase(EdgeRecord.find(e.p_twin->myaddr));
    
    
}
vector<Edge> DCEL::walkAround(Face f) //CHECKED
{
    vector<Edge> ans;
    Edge start=*(f.p_incident);
    ans.push_back(start);
    Edge current=*(start.p_next);
    while(current!=start)
    {
        //cout<<"$$$\n";
        //cout<<current.myaddr<<endl;
        ans.push_back(current);
        current=*(current.p_next);
    }
    return ans;
}
vector<Face> DCEL::incidentFaces(Vertex v)
{
    vector<Face> ans;
    Edge start=*(v.p_incident);
    Edge current=*(start.p_twin);
    ans.push_back(*(start.p_incident));
    while(current!=start)
    {
        ans.push_back(*(current.p_incident));
        current=*(current.p_next);
        if(current==start)
            break;
        current=*(current.p_twin);
    }
    return ans;
}
vector<Vertex> DCEL::generateFaceVertices(Face f)
{
    vector<Edge> edges=walkAround(f);
   // cout<<edges.size()<<endl;
    vector<Vertex> ans;
    for(vector<Edge>::iterator it=edges.begin();it!=edges.end();it++)
    {
        ans.push_back(*(it->p_start));
    }
    return ans;
}
void DCEL::flipEdge(Edge &e)
{
    //Vertex Record Changes
    e.p_start->p_incident=e.p_twin->p_next;
    e.p_twin->p_start->p_incident=e.p_next;
    //Face Record Changes
    e.p_incident->p_incident=e.p_prev;
    e.p_twin->p_incident->p_incident=e.p_twin->p_prev;
    //Edge Pointer Changes
    Edge new_u;
    Edge new_d;
    new_u.p_start=e.p_prev->p_start;
    new_u.p_incident=e.p_twin->p_incident;
    new_u.p_next=e.p_twin->p_prev;
    new_u.p_prev=e.p_next;
    new_u.p_twin=&EdgeRecord[e.myaddr];
    
    new_d.p_start=e.p_twin->p_prev->p_start;
    new_d.p_incident=e.p_incident;
    new_d.p_next=e.p_prev;
    new_d.p_prev=e.p_twin->p_next;
    new_d.p_twin=&EdgeRecord[e.p_twin->myaddr];
    
    new_u.p_prev->p_next=&EdgeRecord[e.p_twin->myaddr];
    new_u.p_prev->p_prev=new_u.p_next;
    new_u.p_prev->p_incident=new_u.p_incident;
    new_u.p_next->p_prev=&EdgeRecord[e.p_twin->myaddr];
    new_u.p_next->p_next=new_u.p_prev;
    
    new_d.p_prev->p_next=&EdgeRecord[e.myaddr];
    new_d.p_prev->p_prev=new_d.p_next;
    new_d.p_next->p_prev=&EdgeRecord[e.myaddr];
    new_d.p_next->p_next=new_d.p_prev;
    new_d.p_next->p_incident=new_d.p_incident;
    
    EdgeRecord[e.myaddr].p_start=new_d.p_start;
    EdgeRecord[e.myaddr].p_twin=new_d.p_twin;
    EdgeRecord[e.myaddr].p_next=new_d.p_next;
    EdgeRecord[e.myaddr].p_prev=new_d.p_prev;
    EdgeRecord[e.myaddr].p_incident=new_d.p_incident;
    
    EdgeRecord[e.p_twin->myaddr].p_start=new_u.p_start;
    EdgeRecord[e.p_twin->myaddr].p_twin=new_u.p_twin;
    EdgeRecord[e.p_twin->myaddr].p_next=new_u.p_next;
    EdgeRecord[e.p_twin->myaddr].p_prev=new_u.p_prev;
    EdgeRecord[e.p_twin->myaddr].p_incident=new_u.p_incident;
    cout<<"Flipped\n";
    
}
void DCEL::display()
{
    cout<<"###################VertexRecord##########\n";
    for(unordered_map<int,Vertex>::iterator it=VertexRecord.begin();it!=VertexRecord.end();it++)
    {
        cout<<it->first<<endl;
        cout<<"COORDINATES-->"<<it->second.coord.x<<" "<<it->second.coord.y<<endl;
    }
    cout<<"####################EdgeRecord############\n";
    for(unordered_map<int,Edge>::iterator it=EdgeRecord.begin();it!=EdgeRecord.end();it++)
    {
        cout<<it->first<<"\t"<<it->second.p_next<<"\t "<<&(it->second)<<endl;
        cout<<"Connecting-->"<<it->second.p_start->myaddr<<" ===  "<<it->second.p_twin->p_start->myaddr<<endl;
        cout<<"________________\n";
    }
    cout<<"####################FaceRecord############\n";
    for(unordered_map<int,Face>::iterator it=FaceRecord.begin();it!=FaceRecord.end();it++)
    {
        cout<<it->first<<endl;
        cout<<"Incident on"<<it->second.p_incident->myaddr<<endl;
    }
    
}
void DCEL::output()
{
    cout<<EdgeRecord.size()<<endl;
    for(unordered_map<int,Edge>::iterator it=EdgeRecord.begin();it!=EdgeRecord.end();it++)
    {
        cout<<it->second.p_start->coord.x<<" "<<it->second.p_start->coord.y<<endl;
        cout<<it->second.p_twin->p_start->coord.x<<" "<<it->second.p_twin->p_start->coord.y<<endl;
        
    }
}
