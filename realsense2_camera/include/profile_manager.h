#include <librealsense2/rs.hpp>
#include <sensor_params.h>

#define STREAM_NAME(sip) (static_cast<std::ostringstream&&>(std::ostringstream() << create_graph_resource_name(ros_stream_to_string(sip.first)) << ((sip.second>0) ? std::to_string(sip.second) : ""))).str()

using namespace rs2;
namespace realsense2_camera
{
    typedef std::pair<rs2_stream, int> stream_index_pair;

    class ProfilesManager
    {
        public:
            ProfilesManager(std::shared_ptr<Parameters> parameters);
            virtual bool isWantedProfile(const rs2::stream_profile& profile) = 0;
            virtual void registerProfileParameters(std::vector<stream_profile> all_profiles, std::function<void()> update_sensor_func) = 0;
            virtual bool set_to_defaults(std::map<stream_index_pair, rs2::stream_profile>& default_profiles) = 0;
            bool isTypeExist();
            virtual std::string wanted_profile_string(stream_index_pair sip) = 0;
            static std::string profile_string(const rs2::stream_profile& profile);
            
            void registerSensorQOSParam(std::string template_name, 
                                        std::set<stream_index_pair> unique_sips, 
                                        std::map<stream_index_pair, std::shared_ptr<std::string> >& params, 
                                        std::string value);

            template<class T>
            void registerSensorUpdateParam(std::string template_name, 
                                           std::set<stream_index_pair> unique_sips, 
                                           std::map<stream_index_pair, std::shared_ptr<T> >& params, 
                                           T value, 
                                           std::function<void()> update_sensor_func);
            void addWantedProfiles(std::vector<rs2::stream_profile>& wanted_profiles);
            void clearParameters();
            bool hasSIP(const stream_index_pair& sip) const;
            rmw_qos_profile_t getQOS(const stream_index_pair& sip) const;
            rmw_qos_profile_t getInfoQOS(const stream_index_pair& sip) const;

        protected:
            rclcpp::Logger _logger;
            SensorParams _params;
            std::map<stream_index_pair, std::shared_ptr<bool>> _enabled_profiles;
            std::map<stream_index_pair, std::shared_ptr<std::string>> _profiles_image_qos_str, _profiles_info_qos_str;
            std::vector<rs2::stream_profile> _all_profiles;
            std::vector<std::string> _parameters_names;
    };

    class VideoProfilesManager : public ProfilesManager
    {
        public:
            VideoProfilesManager(std::shared_ptr<Parameters> parameters, const std::string& module_name);
            bool isWantedProfile(const rs2::stream_profile& profile) override;
            void registerProfileParameters(std::vector<stream_profile> all_profiles, std::function<void()> update_sensor_func) override;
            bool set_to_defaults(std::map<stream_index_pair, rs2::stream_profile>& default_profiles);
            // bool isTypeExist();
            std::string wanted_profile_string(stream_index_pair sip);
            int getHeight() {return _height;};
            int getWidth() {return _width;};
            int getFPS() {return _fps;};

        private:
            void registerVideoSensorParams();

        private:
            std::string _module_name;
            std::map<rs2_stream, rs2_format>  _allowed_formats;
            int      _fps;
            int _width, _height;
            bool _is_profile_exist;
    };

    class MotionProfilesManager : public ProfilesManager
    {
        public:
            using ProfilesManager::ProfilesManager;
            bool isWantedProfile(const rs2::stream_profile& profile) override;
            void registerProfileParameters(std::vector<stream_profile> all_profiles, std::function<void()> update_sensor_func) override;
            bool set_to_defaults(std::map<stream_index_pair, rs2::stream_profile>& default_profiles);
            std::string wanted_profile_string(stream_index_pair sip);

        protected:
            std::map<stream_index_pair, std::shared_ptr<double> > _fps;
    };

    class PoseProfilesManager : public MotionProfilesManager
    {
        public:
            using MotionProfilesManager::MotionProfilesManager;
            void registerProfileParameters(std::vector<stream_profile> all_profiles, std::function<void()> update_sensor_func) override;
    };

}