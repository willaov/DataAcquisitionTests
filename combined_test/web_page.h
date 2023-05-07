/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _WEB_PAGE_H_
#define _WEB_PAGE_H_

#define index_page  "<!DOCTYPE html>"\
                    "<html lang=\"en\">"\
                    "<head>"\
                        "<meta charset=\"UTF-8\">"\
                        "<title>HTTP Server Example</title>"\
                    "</head>"\
                    "<body>"\
                        "<h1>Hello, World!</h1>"\
                        "<p>%s</p>"\
                    "</body>"\
                    "</html>"

#define data_page  "<!DOCTYPE html>"\
                    "<html lang=\"en\">"\
                    "<head>"\
                        "<meta charset=\"UTF-8\">"\
                        "<meta http-equiv=\"refresh\" content=\"1\">"\
                        "<title>Data Output</title>"\
                    "</head>"\
                    "<body>"\
                        "<h1>Data From The ADC</h1>"\
                        "<table>"\
                            "<tr>"\
                                "<th>Channel</th>"\
                                "<th>Data/V</th>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>1</td>"\
                                "<td>%.2f</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>2</td>"\
                                "<td>%.2f</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>3</td>"\
                                "<td>%.2f</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>4</td>"\
                                "<td>%.2f</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>5</td>"\
                                "<td>%.2f</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>6</td>"\
                                "<td>%.2f</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>7</td>"\
                                "<td>%.2f</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>8</td>"\
                                "<td>%.2f</td>"\
                            "</tr>"\
                        "</table>"\
                    "</body>"\
                    "</html>"

#define onewire_page  "<!DOCTYPE html>"\
                    "<html lang=\"en\">"\
                    "<head>"\
                        "<meta charset=\"UTF-8\">"\
                        "<title>OneWire</title>"\
                    "</head>"\
                    "<body>"\
                        "<h1>OneWire</h1>"\
                        "<p>Found %d devices</p>"\
                        "<table>"\
                            "<tr>"\
                                "<th>Channel</th>"\
                                "<th>Device Address</th>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>1</td>"\
                                "<td>%s</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>2</td>"\
                                "<td>%s</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>3</td>"\
                                "<td>%s</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>4</td>"\
                                "<td>%s</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>5</td>"\
                                "<td>%s</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>6</td>"\
                                "<td>%s</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>7</td>"\
                                "<td>%s</td>"\
                            "</tr>"\
                            "<tr>"\
                                "<td>8</td>"\
                                "<td>%s</td>"\
                            "</tr>"\
                        "</table>"\
                    "</body>"\
                    "</html>"

#endif /* _WEB_PAGE_H_ */