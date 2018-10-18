#include "ros/ros.h"
#include "sensor_msgs/PointCloud2.h"
#include "std_msgs/String.h"
#include "pcl/point_types.h"
#include "pcl/point_cloud.h"
#include "pcl_conversions/pcl_conversions.h"
#include "pcl/filters/voxel_grid.h"

ros::Publisher pub;

class CloudCombine
{
    public:
        void cloudCallback(const sensor_msgs::PointCloud2ConstPtr& input);
};

void CloudCombine::cloudCallback(const sensor_msgs::PointCloud2ConstPtr& input)
{
    // Containers for data
    pcl::PointCloud<pcl::PointXYZ> cloud;
    pcl::PointCloud<pcl::PointXYZ> cloud_filtered;

    pcl::fromROSMsg(*input, cloud);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_p = cloud.makeShared();

    pcl::VoxelGrid<pcl::PointXYZ> sor;
    sor.setInputCloud (cloud_p);
    sor.setLeafSize (1.0f, 1.0f, 1.0f);
    sor.filter (cloud_filtered);
    
    // Convert to ROS data type
    sensor_msgs::PointCloud2 output;
    pcl::toROSMsg(cloud_filtered, output);
    ROS_INFO("width: %d", output.width);
    ROS_INFO("size: %zu", output.data.size());
    output.header.frame_id = "laser";
    pub.publish(output);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "cloud_combine_server");
    ros::NodeHandle nh;

    CloudCombine c;
    ros::Subscriber sub = nh.subscribe<sensor_msgs::PointCloud2> ("velodyne_points", 1000, &CloudCombine::cloudCallback, &c);
    pub = nh.advertise<sensor_msgs::PointCloud2> ("downsampled_points", 1);

    ros::spin();
    return 0;
}
