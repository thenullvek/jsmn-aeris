#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include "testutil.h"

int test_empty(void) {
  check(parse("{}", JSMN_SUCCESS, 1, JSMN_OBJECT, 0, 0));
  check(parse("[]", JSMN_SUCCESS, 1, JSMN_ARRAY, 0, 0));
  check(parse("[{},{}]", JSMN_SUCCESS, 3, JSMN_ARRAY, 0, 2, JSMN_OBJECT, 1, 0, JSMN_OBJECT, 4, 0));
  return 0;
}

int test_object(void) {
  check(parse("{\"a\":0}", JSMN_SUCCESS, 3, JSMN_OBJECT, 0, 1, JSMN_STRING, "a", 1,
              JSMN_PRIMITIVE, "0"));
  check(parse("{\"a\":[]}", JSMN_SUCCESS, 3, JSMN_OBJECT, 0, 1, JSMN_STRING, "a", 1,
              JSMN_ARRAY, 5, 0));
  check(parse("{\"a\":{},\"b\":{}}", JSMN_SUCCESS, 5, JSMN_OBJECT, -1, 2, JSMN_STRING,
              "a", 1, JSMN_OBJECT, -1, 0, JSMN_STRING, "b", 1, JSMN_OBJECT, -1, 0));
  check(parse("{\n \"Day\": 26,\n \"Month\": 9,\n \"Year\": 12\n }", JSMN_SUCCESS, 7,
              JSMN_OBJECT, -1, 3, JSMN_STRING, "Day", 3, JSMN_PRIMITIVE,
              "26", JSMN_STRING, "Month", 5, JSMN_PRIMITIVE, "9", JSMN_STRING,
              "Year", 4, JSMN_PRIMITIVE, "12"));
  check(parse("{\"a\": 0, \"b\": \"c\"}", JSMN_SUCCESS, 5, JSMN_OBJECT, -1, 2,
              JSMN_STRING, "a", 1, JSMN_PRIMITIVE, "0", JSMN_STRING, "b", 1,
              JSMN_STRING, "c", 1));

#ifdef JSMN_STRICT
  check(parse("{\"a\"\n0}", JSMN_ERROR_INVAL, 3));
  check(parse("{\"a\", 0}", JSMN_ERROR_INVAL, 3));
  check(parse("{\"a\": {2}}", JSMN_ERROR_INVAL, 3));
  check(parse("{\"a\": {2: 3}}", JSMN_ERROR_INVAL, 3));
  check(parse("{\"a\": {\"a\": 2 3}}", JSMN_ERROR_INVAL, 5));

  check(parse("{\"a\"}", JSMN_ERROR_INVAL, 2));
  check(parse("{\"a\": 1, \"b\"}", JSMN_ERROR_INVAL, 4));
  check(parse("{\"a\",\"b\":1}", JSMN_ERROR_INVAL, 4));
  check(parse("{\"a\":1,}", JSMN_ERROR_INVAL, 4));
  check(parse("{\"a\":\"b\":\"c\"}", JSMN_ERROR_INVAL, 4));
  check(parse("{,}", JSMN_ERROR_INVAL, 4));
#endif
  return 0;
}

int test_array(void) {
  check(parse("[10}", JSMN_ERROR_INVAL, 3));
  check(parse("[1,,3]", JSMN_ERROR_INVAL, 3));
  check(parse("[10]", JSMN_SUCCESS, 2, JSMN_ARRAY, -1, 1, JSMN_PRIMITIVE, "10"));
  check(parse("{\"a\": 1]", JSMN_ERROR_INVAL, 3));
  check(parse("[\"a\": 1]", JSMN_ERROR_INVAL, 3));
  return 0;
}

int test_primitive(void) {
  check(parse("{\"boolVar\" : true }", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1,
              JSMN_STRING, "boolVar", 7, JSMN_PRIMITIVE, "true"));
  check(parse("{\"boolVar\" : false }", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1,
              JSMN_STRING, "boolVar", 7, JSMN_PRIMITIVE, "false"));
  check(parse("{\"nullVar\" : null }", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1,
              JSMN_STRING, "nullVar", 7, JSMN_PRIMITIVE, "null"));
  check(parse("{\"intVar\" : 12}", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1, JSMN_STRING,
              "intVar", 6, JSMN_PRIMITIVE, "12"));
  check(parse("{\"floatVar\" : 12.345}", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1,
              JSMN_STRING, "floatVar", 8, JSMN_PRIMITIVE, "12.345"));
  return 0;
}

int test_string(void) {
  check(parse("{\"strVar\" : \"hello world\"}", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1,
              JSMN_STRING, "strVar", 6, JSMN_STRING, "hello world", 11));
  check(parse("{\"strVar\" : \"escapes: \\/\\r\\n\\t\\b\\f\\\"\\\\\"}", JSMN_SUCCESS, 3,
              JSMN_OBJECT, -1, 1, JSMN_STRING, "strVar", 6, JSMN_STRING,
              "escapes: \\/\\r\\n\\t\\b\\f\\\"\\\\", 25));
  check(parse("{\"strVar\": \"\"}", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1, JSMN_STRING,
              "strVar", 6, JSMN_STRING, "", 0));
  check(parse("{\"a\":\"\\uAbcD\"}", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1, JSMN_STRING,
              "a", 1, JSMN_STRING, "\\uAbcD", 6));
  check(parse("{\"a\":\"str\\u0000\"}", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1,
              JSMN_STRING, "a", 1, JSMN_STRING, "str\\u0000", 9));
  check(parse("{\"a\":\"\\uFFFFstr\"}", JSMN_SUCCESS, 3, JSMN_OBJECT, -1, 1,
              JSMN_STRING, "a", 1, JSMN_STRING, "\\uFFFFstr", 9));
  check(parse("{\"a\":[\"\\u0280\"]}", JSMN_SUCCESS, 4, JSMN_OBJECT, -1, 1,
              JSMN_STRING, "a", 1, JSMN_ARRAY, -1, 1, JSMN_STRING,
              "\\u0280", 6));

  check(parse("{\"a\":\"str\\uFFGFstr\"}", JSMN_ERROR_INVAL, 3));
  check(parse("{\"a\":\"str\\u@FfF\"}", JSMN_ERROR_INVAL, 3));
  check(parse("{{\"a\":[\"\\u028\"]}", JSMN_ERROR_INVAL, 4));
  return 0;
}

int test_partial_string(void) {
  int r;
  unsigned long i;
  jsmn_parser p;
  jsmntok_t tok[5];
  const char *js = "{\"x\": \"va\\\\ue\", \"y\": \"value y\"}";

  jsmn_init(&p);
  for (i = 1; i <= strlen(js); i++) {
    r = jsmn_parse(&p, js, i, tok, sizeof(tok) / sizeof(tok[0]));
    if (i == strlen(js)) {
      check(r == JSMN_SUCCESS);
      check(tokeq(js, tok, 5, JSMN_OBJECT, -1, 2, JSMN_STRING, "x", 1,
                  JSMN_STRING, "va\\\\ue", 6, JSMN_STRING, "y", 1, JSMN_STRING,
                  "value y", 7));
    } else {
      check(r != JSMN_SUCCESS);
    }
  }
  return 0;
}

int test_partial_array(void) {
  int r;
  unsigned long i;
  jsmn_parser p;
  jsmntok_t tok[10];
  const char *js = "[ 1, true, [123, \"hello\"]]";

  jsmn_init(&p);
  for (i = 1; i <= strlen(js); i++) {
    r = jsmn_parse(&p, js, i, tok, sizeof(tok) / sizeof(tok[0]));
    if (i == strlen(js)) {
      check(r == JSMN_SUCCESS);
      check(tokeq(js, tok, 6, JSMN_ARRAY, -1, 3, JSMN_PRIMITIVE, "1", JSMN_PRIMITIVE, "true",
                  JSMN_ARRAY, -1, 2, JSMN_PRIMITIVE, "123", JSMN_STRING, "hello", 5));
    } else {
      check(r != JSMN_SUCCESS);
    }
  }
  return 0;
}

int test_array_nomem(void) {
  int i;
  int r;
  jsmn_parser p;
  jsmntok_t toksmall[10], toklarge[10];
  const char *js;

  js = "  [ 1, true, [123, \"hello\"]]";

  for (i = 0; i < 6; i++) {
    jsmn_init(&p);
    memset(toksmall, 0, sizeof(toksmall));
    memset(toklarge, 0, sizeof(toklarge));
    r = jsmn_parse(&p, js, strlen(js), toksmall, i);
    check(r == JSMN_ERROR_NOMEM);

    memcpy(toklarge, toksmall, sizeof(toksmall));

    r = jsmn_parse(&p, js, strlen(js), toklarge, 10);
    check(r == JSMN_SUCCESS);
    check(tokeq(js, toklarge, 4, JSMN_ARRAY, -1, 3, JSMN_PRIMITIVE, "1",
                JSMN_PRIMITIVE, "true", JSMN_ARRAY, -1, 2, JSMN_PRIMITIVE,
                "123", JSMN_STRING, "hello", 5));
  }
  return 0;
}

int test_unquoted_keys(void) {
  int r;
  jsmn_parser p;
  jsmntok_t tok[10];
  const char *js;

  jsmn_init(&p);
  js = "key1: \"value\"\nkey2 : 123";

  r = jsmn_parse(&p, js, strlen(js), tok, 10);
  check(r != JSMN_SUCCESS);
  return 0;
}

int test_issue_22(void) {
  int r;
  jsmn_parser p;
  jsmntok_t tokens[128];
  const char *js;

  js =
      "{ \"height\":10, \"layers\":[ { \"data\":[6,6], \"height\":10, "
      "\"name\":\"Calque de Tile 1\", \"opacity\":1, \"type\":\"tilelayer\", "
      "\"visible\":true, \"width\":10, \"x\":0, \"y\":0 }], "
      "\"orientation\":\"orthogonal\", \"properties\": { }, \"tileheight\":32, "
      "\"tilesets\":[ { \"firstgid\":1, \"image\":\"..\\/images\\/tiles.png\", "
      "\"imageheight\":64, \"imagewidth\":160, \"margin\":0, "
      "\"name\":\"Tiles\", "
      "\"properties\":{}, \"spacing\":0, \"tileheight\":32, \"tilewidth\":32 "
      "}], "
      "\"tilewidth\":32, \"version\":1, \"width\":10 }";
  jsmn_init(&p);
  r = jsmn_parse(&p, js, strlen(js), tokens, 128);
  check(r == JSMN_SUCCESS && p.toknext > 10);
  return 0;
}

int test_issue_27(void) {
  const char *js =
      "{ \"name\" : \"Jack\", \"age\" : 27 } { \"name\" : \"Anna\", ";
  check(parse(js, JSMN_ERROR_EXPECTED_EOF, 8));
  return 0;
}

int test_input_length(void) {
  const char *js;
  int r;
  jsmn_parser p;
  jsmntok_t tokens[10];

  js = "{\"a\": 0}garbage";

  jsmn_init(&p);
  r = jsmn_parse(&p, js, 8, tokens, 10);
  check(r == JSMN_SUCCESS);
  check(tokeq(js, tokens, 3, JSMN_OBJECT, -1, 1, JSMN_STRING, "a", 1,
              JSMN_PRIMITIVE, "0"));
  return 0;
}

int test_unmatched_brackets(void) {
  const char *js;
  js = "\"key 1\": 1234}";
  check(parse(js, JSMN_ERROR_INVAL, 2));
  js = "{\"key 1\": 1234\n";
  check(parse(js, JSMN_ERROR_UNCLOSED_OBJECT, 3));
  js = "{\"key 1\": 1234}}";
  check(parse(js, JSMN_ERROR_INVAL, 3));
  js = "\"key 1\"}: 1234";
  check(parse(js, JSMN_ERROR_INVAL, 3));
  js = "{\"key {1\": 1234}";
  check(parse(js, JSMN_SUCCESS, 3, JSMN_OBJECT, 0, 1, JSMN_STRING, "key {1", 6,
              JSMN_PRIMITIVE, "1234"));
  js = "{\"key 1\":{\"key 2\": 1234}\n";
  check(parse(js, JSMN_ERROR_UNCLOSED_OBJECT, 5));
  return 0;
}

int test_object_key(void) {
  const char *js;

  js = "{\"key\": 1}";
  check(parse(js, JSMN_SUCCESS, 3, JSMN_OBJECT, 0, 1, JSMN_STRING, "key", 3,
              JSMN_PRIMITIVE, "1"));
  js = "{true: 1}";
  check(parse(js, JSMN_ERROR_INVAL, 3));
  js = "{1: 1}";
  check(parse(js, JSMN_ERROR_INVAL, 3));
  js = "{{\"key\": 1}: 2}";
  check(parse(js, JSMN_ERROR_INVAL, 5));
  js = "{[1,2]: 2}";
  check(parse(js, JSMN_ERROR_INVAL, 5));
  return 0;
}

int test_bad_assignment(void) {
  const char *js;

  js = "{\"a\": \"bcd\" \"efg\"}";
  check(parse(js, JSMN_ERROR_UNEXPECTED_CHAR, 4));
  js = "{\"a\": 123 32}";
  check(parse(js, JSMN_ERROR_UNEXPECTED_CHAR, 4));
  return 0;
}

int main(void) {
  test(test_empty, "test for a empty JSON objects/arrays");
  test(test_object, "test for a JSON objects");
  test(test_array, "test for a JSON arrays");
  test(test_primitive, "test primitive JSON data types");
  test(test_string, "test string JSON data types");

  test(test_partial_string, "test partial JSON string parsing");
  test(test_partial_array, "test partial array reading");
  test(test_array_nomem, "test array reading with a smaller number of tokens");
  test(test_unquoted_keys, "test unquoted keys (like in JavaScript)");
  test(test_input_length, "test strings that are not null-terminated");
  test(test_issue_22, "test issue #22");
  test(test_issue_27, "test issue #27");
  test(test_unmatched_brackets, "test for unmatched brackets");
  test(test_object_key, "test for key type");

  test(test_bad_assignment, "test for malformed attribute assignment");

  printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);
  return (test_failed > 0);
}
