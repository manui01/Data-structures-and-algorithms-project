// Datastructures.hh
//
// Student name: Manu Ikola

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <functional>
#include <exception>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <unordered_set>

using namespace std;

// Types for IDs
using AffiliationID = std::string;
using PublicationID = unsigned long long int;
using Name = std::string;
using Year = unsigned short int;
using Weight = int;
struct Connection;
// Type for a distance (in arbitrary units)
using Distance = int;

using Path = std::vector<Connection>;
using PathWithDist = std::vector<std::pair<Connection,Distance>>;

// Return values for cases where required thing was not found
AffiliationID const NO_AFFILIATION = "---";
PublicationID const NO_PUBLICATION = -1;
Name const NO_NAME = "!NO_NAME!";
Year const NO_YEAR = -1;
Weight const NO_WEIGHT = -1;

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};


// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

struct Connection
{
    AffiliationID aff1 = NO_AFFILIATION;
    AffiliationID aff2 = NO_AFFILIATION;
    Weight weight = NO_WEIGHT;
    bool operator==(const Connection& c1) const{
        return (aff1==c1.aff1) && (aff2==c1.aff2);
    }
};

// Hash function so Affiliation objects can be stored inside unordered_(set/map).
// also makes sure there is no duplicates of the same connection.
struct ConnectionHash {
    std::size_t operator()(const Connection& c) const {

            AffiliationID affiliation1 = c.aff1;
            AffiliationID affiliation2 = c.aff2;
            if (affiliation1 > affiliation2) {
                std::swap(affiliation1, affiliation2);
            }

            // Hashing
            std::size_t hash1 = std::hash<std::string>{}(affiliation1);
            std::size_t hash2 = std::hash<std::string>{}(affiliation2);

            return hash1 ^ (hash2 << 1);
        }
};


const Connection NO_CONNECTION{NO_AFFILIATION,NO_AFFILIATION,NO_WEIGHT};


// Return value for cases where Distance is unknown
Distance const NO_DISTANCE = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    NotImplemented() : msg_{} {}
    explicit NotImplemented(std::string const& msg) : msg_{msg + " not implemented"} {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};


//  Struct for Affiliation and publications. Since the different functions require
// lot of searching with AffiliationID or PublicationID, the id is stored as the key
// of the unordered_map and not inside the struct object.
struct Affiliation {
    Name name;
    Coord location;
    vector<PublicationID> related_publications;
};

// Tree data structure for the publications.
struct Publication {
    Name title;
    Year year;
    vector<AffiliationID> related_affiliations;
    vector<PublicationID> references;
    shared_ptr<PublicationID> parent;
};


class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate: size() operation has time complexity of O(1)
    unsigned int get_affiliation_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate: clear() operation takes linear time.
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate: For loop goes through each element.
    std::vector<AffiliationID> get_all_affiliations();

    // Estimate of performance: O(1)
    // Short rationale for estimate: Adding new object to unordered_map takes constant time.
    // Adding new object takes constant time.
    bool add_affiliation(AffiliationID id, Name const& name, Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Searching from unordered_map can
    // be O(n) at worst.
    Name get_affiliation_name(AffiliationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Searching from unordered_map can
    // be O(n) at worst.
    Coord get_affiliation_coord(AffiliationID id);


    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n*log(n))
    // Short rationale for estimate: Inserting element into map has time complexity of log(n).
    // doing that n times has time complexity of n*log(n)
    std::vector<AffiliationID> get_affiliations_alphabetically();

    // Estimate of performance: O(n*log(n))
    // Short rationale for estimate: Sorting algorithm has time complexity of n*log(n).
    std::vector<AffiliationID> get_affiliations_distance_increasing();

    // Estimate of performance: O(n)
    // Short rationale for estimate: For loop goes through each element.
    AffiliationID find_affiliation_with_coord(Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Searching from unordered_map can
    // be O(n) at worst.
    bool change_affiliation_coord(AffiliationID id, Coord newcoord);


    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate: Copying affiliations vector is O(n) operation.
    bool add_publication(PublicationID id, Name const& name, Year year, const std::vector<AffiliationID> & affiliations);

    // Estimate of performance: O(n)
    // Short rationale for estimate: For loop goes through each element.
    std::vector<PublicationID> all_publications();

    // Estimate of performance: O(1)
    // Short rationale for estimate: Searching with key from unordered_map takes constant time.
    Name get_publication_name(PublicationID id);

    // Estimate of performance: O(1)
    // Short rationale for estimate: Searching with key from unordered_map takes constant time.
    Year get_publication_year(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Copying vector is O(n) operation.
    std::vector<AffiliationID> get_affiliations(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Searching twice from unordered_map
    // and adding data to the struct objects can be O(n) at worst.
    bool add_reference(PublicationID id, PublicationID parentid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Copying vector is O(n) opertaion
    std::vector<PublicationID> get_direct_references(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: 2 Searches from unordered_map can be O(n)
    bool add_affiliation_to_publication(AffiliationID affiliationid, PublicationID publicationid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Copying vector is O(n)
    std::vector<PublicationID> get_publications(AffiliationID id);

    // Estimate of performance: O(1)
    // Short rationale for estimate: Searching from unordered_map
    PublicationID get_parent(PublicationID id);

    // Estimate of performance: O(n*log(n))
    // Short rationale for estimate: The sorting algorithm has time complexity of O(n*log(n))
    std::vector<std::pair<Year, PublicationID>> get_publications_after(AffiliationID affiliationid, Year year);

    // Estimate of performance: O(n)
    // Short rationale for estimate: While loop goes through each reference
    // chain object.
    std::vector<PublicationID> get_referenced_by_chain(PublicationID id);


    // Non-compulsory operations

    // Estimate of performance: O(1)
    // Short rationale for estimate: Finding from unordered_map takes constant time.
    std::vector<PublicationID> get_all_references(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: For loop goes through each element.
    std::vector<AffiliationID> get_affiliations_closest_to(Coord xy);

    // Estimate of performance: O(1)
    // Short rationale for estimate: Removing from unordered_map takes constant time.
    bool remove_affiliation(AffiliationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: For loop goes through each element.
    PublicationID get_closest_common_parent(PublicationID id1, PublicationID id2);

    // Estimate of performance: O(1)
    // Short rationale for estimate: Removing from unordered_map takes constant time.
    bool remove_publication(PublicationID publicationid);

    // PRG 2 functions:

    std::vector<Connection> get_connected_affiliations(AffiliationID id);

    std::vector<Connection> get_all_connections();

    Path get_any_path(AffiliationID source, AffiliationID target);


private:
    std::unordered_map<AffiliationID, Affiliation> affiliations_;
    std::unordered_map<PublicationID, Publication> publications;
    std::unordered_set<Connection, ConnectionHash> connections;
    Weight count_common_elements(const std::vector<PublicationID> vec1, const std::vector<PublicationID> vec2);
    Connection swap_affiliation_ids(Connection conn);
};

#endif // DATASTRUCTURES_HH
