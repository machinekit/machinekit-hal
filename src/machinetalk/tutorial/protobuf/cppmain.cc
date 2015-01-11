#include <stdio.h>
#include <string>
#include <unistd.h>

#include <google/protobuf/text_format.h>
#include <json2pb.hh>
#include <demo.pb.h>   // autogenerated from demo.proto

using namespace std;
using namespace google::protobuf;

void ptarget(const char *tag, const pb::Target &t)
{
    if (t.has_x()) printf("%sx = %f\n", tag, t.x());
    if (t.has_y()) printf("%sy = %f\n", tag, t.y());
    if (t.has_z()) printf("%sz = %f\n", tag, t.z());
}

void process(pb::DemoContainer &d)
{
    printf("Container type=%d name=%s\n", d.type(), DemoType_Name(d.type()).c_str() );
    if (d.has_velocity()) printf("default velocity %f\n", d.velocity());
    if (d.has_acceleration()) printf("default acceleration %f\n", d.acceleration());

    for (int i = 0; i < d.segment_size(); i++) {
	const pb::Segment &s = d.segment().Get(i);
	printf("\tSegment type=%d name=%s\n", s.type(), SegmentType_Name(s.type()).c_str() );
	if (s.has_end()) ptarget("\t\tend.", s.end());
	if (s.has_normal()) ptarget("\t\tnormal.", s.normal());
	if (s.has_center()) ptarget("\t\tcenter.", s.center());
    }
}

int main(int argc, char* argv[])
{

    if (argc < 2)
	exit(1);

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    char b[10000];
    ssize_t len;

    len = read(0, b, sizeof(b));
    if (len < 0)
	exit(1);

    string buffer  = string(b,len);
    pb::DemoContainer d, got;

    if (!strcmp(argv[1],"binary")) {
	if (!d.ParseFromString(buffer)) {
	    fprintf(stderr, "Failed to ParseFromString message\n");
	    exit(1);
	}
	process(d);
    }

    if (!strcmp(argv[1],"text")) {
	if (!TextFormat::ParseFromString(buffer, &d)){
	    fprintf(stderr, "Failed to ParseFromString message\n");
	    exit(1);
	}
	process(d);
    }

    if (!strcmp(argv[1],"json")) {
	try {
	    json2pb(d, b, len);
	} catch (std::exception &ex) {
	    printf("json2pb exception: %s\n", ex.what());
	    exit(1);
	}
	process(d);
    }

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}