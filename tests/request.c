#include <smolrtsp/request.h>

#include "nala.h"

TEST(deserialize_request) {
    const SmolRTSP_Request expected = {
        .start_line =
            {
                .method = SMOLRTSP_METHOD_DESCRIBE,
                .uri = Slice99_from_str("http://example.com"),
                .version = {.major = 1, .minor = 1},
            },
        .header_map =
            {
                .len = 3,
                .size = 3,
                .headers =
                    (SmolRTSP_Header[]){
                        {
                            SMOLRTSP_HEADER_NAME_CONTENT_LENGTH,
                            Slice99_from_str("10"),
                        },
                        {
                            SMOLRTSP_HEADER_NAME_ACCEPT_LANGUAGE,
                            Slice99_from_str("English"),
                        },
                        {
                            SMOLRTSP_HEADER_NAME_CONTENT_TYPE,
                            Slice99_from_str("application/octet-stream"),
                        },
                    },
            },
        .body = Slice99_from_str("0123456789"),
    };

    SmolRTSP_RequestDeserializerState state = SMOLRTSP_REQUEST_DESERIALIZER_START_STATE;
    SmolRTSP_Request result = {
        .header_map =
            {
                .headers = (SmolRTSP_Header[3]){0},
                .len = 0,
                .size = 3,
            },
    };
    SmolRTSP_DeserializeResult res;

    res = SmolRTSP_Request_deserialize(
        &result, (Slice99[]){Slice99_from_str("DESCRIBE http://example.com RTSP/1.1\r\n")}, &state);
    ASSERT_EQ(res, SmolRTSP_DeserializeResultPending);
    ASSERT_EQ(state.tag, SmolRTSP_RequestDeserializerStateHeaderMap);
    assert(SmolRTSP_RequestLine_eq(result.start_line, expected.start_line));

    res = SmolRTSP_Request_deserialize(
        &result, (Slice99[]){Slice99_from_str("Content-Length: 10\r\n")}, &state);
    ASSERT_EQ(res, SmolRTSP_DeserializeResultPending);
    ASSERT_EQ(state.tag, SmolRTSP_RequestDeserializerStateHeaderMap);
    assert(SmolRTSP_Header_eq(result.header_map.headers[0], expected.header_map.headers[0]));

    res = SmolRTSP_Request_deserialize(
        &result, (Slice99[]){Slice99_from_str("Accept-Language: English\r\n")}, &state);
    ASSERT_EQ(res, SmolRTSP_DeserializeResultPending);
    ASSERT_EQ(state.tag, SmolRTSP_RequestDeserializerStateHeaderMap);
    assert(SmolRTSP_Header_eq(result.header_map.headers[1], expected.header_map.headers[1]));

    res = SmolRTSP_Request_deserialize(
        &result, (Slice99[]){Slice99_from_str("Content-Type: application/octet-stream\r\n")},
        &state);
    ASSERT_EQ(res, SmolRTSP_DeserializeResultPending);
    ASSERT_EQ(state.tag, SmolRTSP_RequestDeserializerStateHeaderMap);
    assert(SmolRTSP_Header_eq(result.header_map.headers[2], expected.header_map.headers[2]));

    res = SmolRTSP_Request_deserialize(
        &result, (Slice99[]){Slice99_from_str("\r\n0123456789")}, &state);
    ASSERT_EQ(res, SmolRTSP_DeserializeResultOk);
    ASSERT_EQ(state.tag, SmolRTSP_RequestDeserializerStateDone);
    assert(SmolRTSP_Request_eq(result, expected));
}