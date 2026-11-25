#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <string>
#include <cmath>

namespace DisasterCore {
namespace Sensors {

class SensorFailureException : public std::exception {
public: const char* what() const noexcept override { return "Sensor failure: telemetry could not be read."; }
};
class TelemetryChecksumMismatch : public std::exception {
public: const char* what() const noexcept override { return "Telemetry checksum mismatch."; }
};
class ThresholdOverflowException : public std::exception {
public: const char* what() const noexcept override { return "Hazard value is outside the supported range."; }
};

inline double computeHazardIndex(double rainfallMmHr, double slopeDegrees) {
    return std::min(100.0, rainfallMmHr * 0.55 + slopeDegrees * 0.85);
}
struct SoilVibrationTelemetry { double soilSaturationPct; double vibrationHz; };
inline double computeHazardIndex(const SoilVibrationTelemetry& t) {
    return std::min(100.0, t.soilSaturationPct * 0.65 + t.vibrationHz * 8.0);
}
inline double computeHazardIndex(double rainfallMmHr, double riverLevelM, double soilSaturationPct) {
    return std::min(100.0, rainfallMmHr * 0.30 + riverLevelM * 12.0 + soilSaturationPct * 0.40);
}

struct GeoCoordinate { double latitude{0}; double longitude{0}; };
inline GeoCoordinate& normalizeCoordinate(GeoCoordinate& c) {
    c.latitude = std::max(-90.0, std::min(90.0, c.latitude));
    c.longitude = std::max(-180.0, std::min(180.0, c.longitude));
    return c;
}

class TelemetryBuffer {
    double* values_;
    std::size_t size_;
public:
    explicit TelemetryBuffer(std::size_t n = 0) : values_(n ? new double[n] : nullptr), size_(n) {
        for (std::size_t i=0;i<size_;++i) values_[i]=0.0;
    }
    TelemetryBuffer(const TelemetryBuffer& other) : values_(other.size_ ? new double[other.size_] : nullptr), size_(other.size_) {
        for (std::size_t i=0;i<size_;++i) values_[i]=other.values_[i];
    }
    TelemetryBuffer& operator=(const TelemetryBuffer& other) {
        if (this != &other) {
            double* p=other.size_ ? new double[other.size_] : nullptr;
            for (std::size_t i=0;i<other.size_;++i) p[i]=other.values_[i];
            delete[] values_; values_=p; size_=other.size_;
        }
        return *this;
    }
    ~TelemetryBuffer(){ delete[] values_; }
    double& operator[](std::size_t i){ if(i>=size_) throw std::out_of_range("buffer index"); return values_[i]; }
    const double& operator[](std::size_t i) const { if(i>=size_) throw std::out_of_range("buffer index"); return values_[i]; }
    std::size_t size() const { return size_; }
};

class TelemetryAuditor;
class SensorStation {
    std::string id_, region_;
    GeoCoordinate coordinate_;
    double rainfall_, soil_, vibration_, riverLevel_, hazard_;
    friend class TelemetryAuditor;
public:
    static int activeStations;
    SensorStation(std::string id="ST-01", std::string region="Himalayan Zone", GeoCoordinate c={})
      : id_(std::move(id)), region_(std::move(region)), coordinate_(c), rainfall_(0),soil_(0),vibration_(0),riverLevel_(0),hazard_(0) { ++activeStations; }
    SensorStation(const SensorStation& s): id_(s.id_),region_(s.region_),coordinate_(s.coordinate_),rainfall_(s.rainfall_),soil_(s.soil_),vibration_(s.vibration_),riverLevel_(s.riverLevel_),hazard_(s.hazard_){++activeStations;}
    ~SensorStation(){--activeStations;}
    void update(double rain,double soil,double vib,double river) {
        if(rain<0 || soil<0 || soil>100 || vib<0 || river<0) throw ThresholdOverflowException();
        rainfall_=rain; soil_=soil; vibration_=vib; riverLevel_=river;
        hazard_=computeHazardIndex(rain,river,soil);
    }
    double hazard() const { return hazard_; }
    const std::string& id() const { return id_; }
    const std::string& region() const { return region_; }
    friend std::ostream& operator<<(std::ostream& os,const SensorStation& s){
        return os<<std::left<<std::setw(10)<<s.id_<<std::setw(18)<<s.region_<<std::setw(10)<<s.rainfall_<<std::setw(10)<<s.soil_<<std::setw(10)<<s.vibration_<<std::setw(10)<<s.riverLevel_<<std::setw(10)<<s.hazard_;
    }
    friend std::istream& operator>>(std::istream& is,SensorStation& s){
        std::cout<<"Enter rainfall(mm/hr), soil saturation(%), vibration(Hz), river level(m): ";
        return is>>s.rainfall_>>s.soil_>>s.vibration_>>s.riverLevel_;
    }
};
int SensorStation::activeStations=0;

class TelemetryAuditor {
public:
    static void inspect(const SensorStation& s){
        std::cout<<"[AUDIT] Station "<<s.id_<<" raw values: rain="<<s.rainfall_<<", soil="<<s.soil_<<", vibration="<<s.vibration_<<", river="<<s.riverLevel_<<"\n";
    }
};

class DisasterTelemetryDevice { public: virtual ~DisasterTelemetryDevice()=default; virtual std::string deviceInfo() const=0; };
class SolarPoweredNode : virtual public DisasterTelemetryDevice { public: std::string deviceInfo() const override { return "Solar-powered telemetry node"; } };
class WirelessLoRaTransceiver : virtual public DisasterTelemetryDevice { public: std::string deviceInfo() const override { return "LoRa wireless telemetry node"; } };
class PowerSubsystem { double charge_; public: explicit PowerSubsystem(double c=100):charge_(c){} bool available() const{return charge_>5;} void consume(double amount){charge_=std::max(0.0,charge_-amount);} double charge() const{return charge_;} };

class BaseSensorNode : public virtual DisasterTelemetryDevice {
protected: PowerSubsystem power_; TelemetryBuffer buffer_{4};
public:
    virtual void readTelemetry(SensorStation& station)=0;
    virtual bool evaluateThreshold(const SensorStation& station) const=0;
    virtual ~BaseSensorNode()=default;
};
class AutonomousHybridProbe : public SolarPoweredNode, public WirelessLoRaTransceiver, public BaseSensorNode {
public:
    std::string deviceInfo() const override { return "Autonomous hybrid probe (solar + LoRa)"; }
    void readTelemetry(SensorStation& s) override {
        if(!power_.available()) throw SensorFailureException();
        // Fixed demo values: this is not connected to hardware.
        s.update(65.0,82.0,2.1,3.8); power_.consume(1.0);
        buffer_[0]=65; buffer_[1]=82; buffer_[2]=2.1; buffer_[3]=3.8;
    }
    bool evaluateThreshold(const SensorStation& s) const override { return s.hazard()>=40.0; }
};

} // Sensors
namespace Alerts {
class EarlyWarningSiren {
    int level_;
    std::string region_;
    static int globalLevel;
public:
    EarlyWarningSiren(std::string r="Himalayan Zone"):level_(0),region_(std::move(r)){}
    void setLevel(int l){level_=std::max(0,std::min(3,l));globalLevel=std::max(globalLevel,level_);}
    int level() const{return level_;}
    static int networkLevel(){return globalLevel;}
    static std::string levelName(int l){switch(l){case 0:return "NORMAL";case 1:return "WATCH";case 2:return "WARNING";default:return "EVACUATION ALERT";}}
    void dispatch() const { if(level_==0) std::cout<<"["<<region_<<"] No alert.\n"; else std::cout<<"[SIREN SIMULATION] "<<region_<<": "<<levelName(level_)<<" (Level "<<level_<<")\n"; }
};
int EarlyWarningSiren::globalLevel=0;
struct IncidentRecord { char station[32]; char region[48]; double hazard; int alertLevel; };
inline void saveIncident(const IncidentRecord& rec,const std::string& file="incidents.bin") {
    std::fstream fs(file,std::ios::binary|std::ios::in|std::ios::out);
    if(!fs){ std::ofstream create(file,std::ios::binary); create.close(); fs.open(file,std::ios::binary|std::ios::in|std::ios::out); }
    fs.seekp(0,std::ios::end); const auto start=fs.tellp(); fs.write(reinterpret_cast<const char*>(&rec),sizeof(rec));
    if(!fs){ fs.clear(); fs.seekp(start); IncidentRecord blank{}; fs.write(reinterpret_cast<const char*>(&blank),sizeof(blank)); throw std::runtime_error("Persistence write failed; record rollback attempted."); }
}
inline std::vector<IncidentRecord> loadIncidents(const std::string& file="incidents.bin") {
    std::vector<IncidentRecord> records; std::ifstream fs(file,std::ios::binary); IncidentRecord r{};
    while(fs.read(reinterpret_cast<char*>(&r),sizeof(r))) records.push_back(r);
    return records;
}
inline std::ostream& alertTable(std::ostream& os){return os<<std::left<<std::setw(12)<<"STATION"<<std::setw(20)<<"REGION"<<std::setw(12)<<"HAZARD"<<"LEVEL\n";}
}
}

struct RegionCluster {
    std::string name; double averageHazard{0}; int stations{0};
    RegionCluster operator+(const RegionCluster& rhs) const { RegionCluster c; c.name=name+" + "+rhs.name; c.stations=stations+rhs.stations; c.averageHazard=c.stations?((averageHazard*stations+rhs.averageHazard*rhs.stations)/c.stations):0; return c; }
    bool operator>(const RegionCluster& rhs) const {return averageHazard>rhs.averageHazard;}
};
struct EvacuationPriority { bool operator()(const RegionCluster& a,const RegionCluster& b) const{return a.averageHazard>b.averageHazard;} };

int main(){
    using namespace DisasterCore::Sensors; using namespace DisasterCore::Alerts;
    std::vector<SensorStation> stations;
    stations.emplace_back("ST-01","North Ridge",GeoCoordinate{30.1,78.2});
    stations.emplace_back("ST-02","River Valley",GeoCoordinate{30.3,78.5});
    std::vector<BaseSensorNode*> nodes; AutonomousHybridProbe probe; nodes.push_back(&probe);
    std::map<std::string,RegionCluster> zones;
    std::cout<<"\n=== AUTONOMOUS LANDSLIDE & FLASH-FLOOD EARLY WARNING GRID ===\n";
    std::cout<<"Simulation mode | No real sensors or sirens are connected.\n\n";
    for(std::size_t i=0;i<stations.size();++i){
        try { nodes[i%nodes.size()]->readTelemetry(stations[i]); }
        catch(const SensorFailureException& e){std::cerr<<e.what()<<"\n";continue;}
        catch(const ThresholdOverflowException& e){std::cerr<<e.what()<<"\n";continue;}
        int level=stations[i].hazard()>=70?3:stations[i].hazard()>=50?2:stations[i].hazard()>=30?1:0;
        EarlyWarningSiren siren(stations[i].region()); siren.setLevel(level); siren.dispatch();
        zones[stations[i].region()]={stations[i].region(),stations[i].hazard(),1};
        IncidentRecord record{}; std::strncpy(record.station,stations[i].id().c_str(),sizeof(record.station)-1); std::strncpy(record.region,stations[i].region().c_str(),sizeof(record.region)-1); record.hazard=stations[i].hazard(); record.alertLevel=level;
        try{saveIncident(record);}catch(const std::exception& e){std::cerr<<"Log error: "<<e.what()<<"\n";}
        TelemetryAuditor::inspect(stations[i]);
    }
    std::cout<<"\n"; alertTable(std::cout);
    for(const auto& s:stations){int l=s.hazard()>=70?3:s.hazard()>=50?2:s.hazard()>=30?1:0; std::cout<<std::setw(12)<<s.id()<<std::setw(20)<<s.region()<<std::setw(12)<<std::fixed<<std::setprecision(1)<<s.hazard()<<EarlyWarningSiren::levelName(l)<<"\n";}
    std::vector<RegionCluster> priority; for(const auto& z:zones)priority.push_back(z.second); std::sort(priority.begin(),priority.end(),EvacuationPriority{});
    std::cout<<"\n[Priority zones sorted by hazard]\n"; for(const auto& z:priority)std::cout<<z.name<<" : "<<z.averageHazard<<"\n";
    std::cout<<"Active sensor stations: "<<SensorStation::activeStations<<" | Global emergency level: "<<EarlyWarningSiren::networkLevel()<<"\n";
    const auto saved=loadIncidents(); std::cout<<"Incident records stored on disk: "<<saved.size()<<" (incidents.bin)\n";
    return 0;
}
