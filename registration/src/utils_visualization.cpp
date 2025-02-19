/* Copyright 2019 Ignacio Torroba (torroba@kth.se)
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "registration/utils_visualization.hpp"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <pcl/common/common.h>


using namespace std;
using pcl::visualization::PointCloudColorHandlerCustom;

SubmapsVisualizer::SubmapsVisualizer(pcl::visualization::PCLVisualizer &viewer):
                                    viewer_(viewer){

}


void SubmapsVisualizer::setVisualizer(SubmapsVec& submaps_set, int num){

    // The color we will be using
    float black = 0.0;  // Black
    float white = 1.0 - black;

    // Create one/two vertically separated viewports
    if (num == 2){
        // Second viewport
        viewer_.createViewPort (0.0, 0.0, 0.5, 1.0, vp1_);
        viewer_.createViewPort (0.5, 0.0, 1.0, 1.0, vp2_);
        viewer_.addText ("Results", 10, 15, 16, white, white, white, "info_2", vp2_);
        viewer_.setBackgroundColor (white, white, white, vp2_);
        viewer_.addCoordinateSystem(5.0, "reference_frame", vp2_);
    }
    else{
        viewer_.createViewPort (0.0, 0.0, 1.0, 1.0, vp1_);
    }


    // Adding text descriptions in each viewport
    viewer_.addText ("Ground truth", 10, 15, 16, white, white, white, "info_1", vp1_);

    // Set GT pointclouds
    unsigned int i = 0;
    PointCloudT::Ptr submap_ptr (new PointCloudT);
    for(SubmapObj& submap: submaps_set){
        submap_ptr.reset(new PointCloudT(submap.submap_pcl_));
        pcl::PointXYZ minPt, maxPt;
        pcl::getMinMax3D (*submap_ptr, minPt, maxPt);
        cout << "Min z: " << minPt.z << endl;
        cout << "Max z: " << maxPt.z << endl;

        pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_xyzrgb(new pcl::PointCloud<pcl::PointXYZRGB>);
        
        copyPointCloud(*submap_ptr, *cloud_xyzrgb);
        cloud_xyzrgb->points.resize(submap.submap_pcl_.size());
        
        
        

        for (size_t i = 0; i < submap.submap_pcl_.points.size(); i++) 
        {
            double r  = fabs(submap.submap_pcl_.points[i].z/minPt.z) * 256;
            double g, b;
            g = b = r;
            if (submap.submap_pcl_.points[i].z > -60)
            {   
                r = 256;
                g = 0;
                b = 0;
                std::cout<<"1\n";
            }
            else if(submap.submap_pcl_.points[i].z > -70)
            {
                r = 0;
                g = 256;
                b = 0;
                std::cout<<"2\n";
            }
            else if(submap.submap_pcl_.points[i].z > -80)
            {
                r = 256;
                g = 256;
                b = 0;
                std::cout<<"3\n";
            }
            else if(submap.submap_pcl_.points[i].z > -100)
            {
                r = 10;
                g = 10;
                b = 10;
                std::cout<<"4\n";
            }
            else if(submap.submap_pcl_.points[i].z > -120)
            {
                r = 50;
                g = 0;
                b = 256;
                std::cout<<"5\n";
            }


            // g = 256;

            // std::cout<<", r : "<<r<<", g "<<g<<", b "<<b<<"\n";
            int32_t rgb = (static_cast<uint32_t>(r) << 16 |
            static_cast<uint32_t>(g) << 8 | static_cast<uint32_t>(b));


            cloud_xyzrgb->points[i].rgb = rgb;
            cloud_xyzrgb->points[i].x = submap.submap_pcl_.points[i].x;
            cloud_xyzrgb->points[i].y = submap.submap_pcl_.points[i].y;
            cloud_xyzrgb->points[i].z = submap.submap_pcl_.points[i].z;
        }


        // max_z = submap_ptr->
        // double r = 
        // submap.colors_ = Vector3d(rand() % 256, rand() % 256, rand() % 256);
        // submap.colors_ = Vector3d(rand() % 256, rand() % 256, rand() % 256);
        // PointCloudColorHandlerCustom<PointT> cloud_color(submap_ptr, submap.colors_[0], submap.colors_[1], submap.colors_[2]);
        // viewer_.addPointCloud(submap_ptr, cloud_color, "gt_cloud_" + std::to_string(i), vp1_);

        viewer_.addPointCloud(cloud_xyzrgb, "gt_cloud_" + std::to_string(i), vp1_);

        viewer_.addCoordinateSystem(3.0, submap.submap_tf_, "gt_cloud_" + std::to_string(i), vp1_);
        i++;
    }

    // Set GT trajectory (linearized)
    Vector3i dr_color = Vector3i(rand() % 256, rand() % 256, rand() % 256);
    for(unsigned int j=0; j<submaps_set.size()-1; j++){
        SubmapObj submap_fr = submaps_set.at(j+1);
        Eigen::Vector3d from_ps = submap_fr.submap_tf_.translation().cast<double>();
        SubmapObj submap_to = submaps_set.at(j);
        Eigen::Vector3d to_ps = submap_to.submap_tf_.translation().cast<double>();
        viewer_.addArrow(PointT(from_ps[0],from_ps[1],from_ps[2]), PointT(to_ps[0],to_ps[1],to_ps[2]),
                dr_color[0], dr_color[1], dr_color[2], false, "gt_dr_edge_" + std::to_string(j), vp1_);
    }

    // Set background color
    viewer_.setBackgroundColor (white, white, white, vp1_);

    // Set camera position and orientation
    viewer_.setSize (1920/2, 1080/2);
    viewer_.addCoordinateSystem(5.0, "gt_reference_frame", vp1_);
}


void SubmapsVisualizer::updateVisualizer(const SubmapsVec& submaps_set){

    viewer_.removeAllPointClouds(vp1_);
    viewer_.removeAllCoordinateSystems(vp1_);
    viewer_.addCoordinateSystem(5.0, "reference_frame", vp1_);

    // Update pointclouds
    unsigned int i = 0;
    PointCloudT::Ptr submap_ptr (new PointCloudT);
    for(const SubmapObj& submap: submaps_set){
        submap_ptr.reset(new PointCloudT(submap.submap_pcl_));
        PointCloudColorHandlerCustom<PointT> cloud_color(submap_ptr, submap.colors_[0], submap.colors_[1], submap.colors_[2]);
        viewer_.addPointCloud(submap_ptr, cloud_color, "cloud_" + std::to_string(i), vp1_);
        viewer_.addCoordinateSystem(3.0, submap.submap_tf_, "cloud_" + std::to_string(i), vp1_);
        i++;
    }
    viewer_.spinOnce();
}

void SubmapsVisualizer::addSubmap(const SubmapObj& submap_i, int vp){

    // Add pointclouds
    if(this->traj_cnt_ == NULL){
        this->traj_cnt_ = 0;
    }
    PointCloudT::Ptr submap_ptr (new PointCloudT);
    submap_ptr.reset(new PointCloudT(submap_i.submap_pcl_));
    PointCloudColorHandlerCustom<PointT> cloud_color(submap_ptr, submap_i.colors_[0], submap_i.colors_[1], submap_i.colors_[2]);
    viewer_.addPointCloud(submap_ptr, cloud_color, "cloud_" + std::to_string(this->traj_cnt_), vp);
    viewer_.addCoordinateSystem(3.0, submap_i.submap_tf_, "cloud_" + std::to_string(this->traj_cnt_), vp);
    this->traj_cnt_++;
}

void SubmapsVisualizer::plotMBESPing(const SubmapObj& submap_i, float spam, float n_beams, int vp){

    Eigen::Matrix3f rot = submap_i.submap_tf_.linear();
    Eigen::Quaternionf rot_q = Quaternionf(rot);
    Eigen::Vector3f z_or = rot.col(2).transpose();
    std::vector<Eigen::Quaternionf, Eigen::aligned_allocator<Eigen::Quaternionf> > beam_directions;

    float roll_step = spam/n_beams;
    float pitch = 0.0, yaw = 0.0;
    Eigen::Quaternionf q;
    q = Eigen::AngleAxisf(3.1415, Eigen::Vector3f::UnitX())
        * Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitY())
        * Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitZ());

    rot_q *= q;

    for(int i = -n_beams/2; i<=n_beams/2; i++){
//          std::cout << "step " << i << " and angle " << roll_step*i << std::endl;
        q = Eigen::AngleAxisf(roll_step*i, Eigen::Vector3f::UnitX())
            * Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitY())
            * Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitZ());
        beam_directions.push_back(Eigen::Quaternionf(rot_q * q));
        Eigen::Matrix3f rot_mat = (rot_q * q).toRotationMatrix();
        Eigen::Vector3f z_or = rot_mat.col(2).transpose();
//          std::cout << "Beams directions " << z_or.transpose() << std::endl;

        pcl::ModelCoefficients line_coeff;
        line_coeff.values.resize (6);    // We need 6 values
        line_coeff.values[0] = submap_i.submap_tf_.translation()[0];
        line_coeff.values[1] = submap_i.submap_tf_.translation()[1];
        line_coeff.values[2] = submap_i.submap_tf_.translation()[2];
        line_coeff.values[3] = z_or.x ();
        line_coeff.values[4] = z_or.y ();
        line_coeff.values[5] = z_or.z ();
        viewer_.addLine(line_coeff, "line_" + std::to_string(i), vp);
    }
}


void SubmapsVisualizer::plotPoseGraphG2O(const GraphConstructor& graph, const SubmapsVec& submaps_set){

    // Clean initial graph
    viewer_.removeAllPointClouds(vp1_);
    viewer_.removeAllCoordinateSystems(vp1_);
    viewer_.removeAllShapes(vp1_);
    viewer_.addCoordinateSystem(5.0, "reference_frame", vp1_);

    // Update pointclouds
    unsigned int i = 0;
    PointCloudT::Ptr submap_ptr (new PointCloudT);
    for(const SubmapObj& submap: submaps_set){
        submap_ptr.reset(new PointCloudT(submap.submap_pcl_));
        PointCloudColorHandlerCustom<PointT> cloud_color(submap_ptr, submap.colors_[0], submap.colors_[1], submap.colors_[2]);
        viewer_.addPointCloud(submap_ptr, cloud_color, "cloud_" + std::to_string(i), vp1_);
        viewer_.addCoordinateSystem(3.0, submap.submap_tf_, "cloud_" + std::to_string(i), vp1_);
        i++;
    }

    // Plot initial trajectory estimate
    i = 0;
    Vector3i dr_color = Vector3i(rand() % 256, rand() % 256, rand() % 256);
    for(unsigned int j=0; j<submaps_set.size()-1; j++){
        SubmapObj submap_fr = submaps_set.at(j+1);
        Eigen::Vector3d from_ps = submap_fr.submap_tf_.translation().cast<double>();
        SubmapObj submap_to = submaps_set.at(j);
        Eigen::Vector3d to_ps = submap_to.submap_tf_.translation().cast<double>();
        viewer_.addArrow(PointT(from_ps[0],from_ps[1],from_ps[2]), PointT(to_ps[0],to_ps[1],to_ps[2]),
                dr_color[0], dr_color[1], dr_color[2], false, "final_dr_edge_" + std::to_string(j), vp1_);
    }

    // Plot LC edges
    i = 0;
    Vector3i lc_color = Vector3i(rand() % 256, rand() % 256, rand() % 256);
    for(g2o::EdgeSE3* edgeLC: graph.lcEdges_){
        g2o::VertexSE3* from = static_cast<g2o::VertexSE3*>(edgeLC->vertex(0));
        g2o::VertexSE3* to = static_cast<g2o::VertexSE3*>(edgeLC->vertex(1));
        Eigen::Vector3d from_ps = from->estimate().translation();
        Eigen::Vector3d to_ps = to->estimate().translation();
        viewer_.addArrow(PointT(from_ps[0],from_ps[1],from_ps[2]), PointT(to_ps[0],to_ps[1],to_ps[2]),
                lc_color[0], lc_color[1], lc_color[2], false, "lc_edge_" + std::to_string(i), vp1_);
        i++;
    }

    viewer_.spinOnce();
}

void SubmapsVisualizer::plotPoseGraphCeres(SubmapsVec& submaps_set){

    // Clean initial graph
    viewer_.removeAllPointClouds(vp1_);
    viewer_.removeAllCoordinateSystems(vp1_);
    viewer_.removeAllShapes(vp1_);
    viewer_.addCoordinateSystem(5.0, "reference_frame", vp1_);

    // Update pointclouds
    unsigned int i = 0;
    PointCloudT::Ptr submap_ptr (new PointCloudT);
    for(const SubmapObj& submap: submaps_set){
        submap_ptr.reset(new PointCloudT(submap.submap_pcl_));
        PointCloudColorHandlerCustom<PointT> cloud_color(submap_ptr, submap.colors_[0], submap.colors_[1], submap.colors_[2]);
        viewer_.addPointCloud(submap_ptr, cloud_color, "cloud_" + std::to_string(i), vp1_);
        viewer_.addCoordinateSystem(3.0, submap.submap_tf_, "cloud_" + std::to_string(i), vp1_);
        i++;
    }

    // Plot final trajectory estimate
    Vector3i dr_color = Vector3i(rand() % 256, rand() % 256, rand() % 256);
    for(unsigned int j=0; j<submaps_set.size()-1; j++){
        SubmapObj submap_fr = submaps_set.at(j+1);
        Eigen::Vector3d from_ps = submap_fr.submap_tf_.translation().cast<double>();
        SubmapObj submap_to = submaps_set.at(j);
        Eigen::Vector3d to_ps = submap_to.submap_tf_.translation().cast<double>();
        viewer_.addArrow(PointT(from_ps[0],from_ps[1],from_ps[2]), PointT(to_ps[0],to_ps[1],to_ps[2]),
                dr_color[0], dr_color[1], dr_color[2], false, "final_dr_edge_" + std::to_string(j), vp1_);
    }

    viewer_.spinOnce();
}

void SubmapsVisualizer::addCoordinateSystem(const Eigen::Isometry3f& tf){

    viewer_.addCoordinateSystem(2.0, tf, "sample_" + std::to_string(this->sample_cnt++), vp2_);
}

void SubmapsVisualizer::removeCoordinateSystem(){

    for(int i=0; i<sample_cnt; i++){
        viewer_.removeCoordinateSystem("sample_" + std::to_string(i), vp2_);
    }
    sample_cnt = 0;
    viewer_.spinOnce();
}

void SubmapsVisualizer::visualizeGrid(const grid& grid){

    viewer_.removeAllCoordinateSystems(vp1_);
    unsigned int cnt = 0;
    for(grid_point tf_grid: grid){
        viewer_.addCoordinateSystem(4.0, std::get<0>(tf_grid), "grid_" + std::to_string(cnt++), vp1_);
    }
    viewer_.spinOnce();
}
