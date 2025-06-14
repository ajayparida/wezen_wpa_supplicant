/*
 * Driver interaction with Linux nl80211/cfg80211 - Event processing
 * Copyright (c) 2002-2017, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2007, Johannes Berg <johannes@sipsolutions.net>
 * Copyright (c) 2009-2010, Atheros Communications
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"
#include <netlink/genl/genl.h>

#include "utils/common.h"
#include "utils/eloop.h"
#include "common/qca-vendor.h"
#include "common/qca-vendor-attr.h"
#include "common/brcm_vendor.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "driver_nl80211.h"
#ifdef CFG_INTERFACE
#include "host_rpu_umac_if.h"
#endif

static void
nl80211_control_port_frame_tx_status(struct wpa_driver_nl80211_data *drv,
				     const u8 *frame, size_t len,
				     struct nlattr *ack, struct nlattr *cookie);


static const char * nl80211_command_to_string(enum nl80211_commands cmd)
{
#define C2S(x) case x: return #x;
	switch (cmd) {
	C2S(NL80211_CMD_UNSPEC)
	C2S(NL80211_CMD_GET_WIPHY)
	C2S(NL80211_CMD_SET_WIPHY)
	C2S(NL80211_CMD_NEW_WIPHY)
	C2S(NL80211_CMD_DEL_WIPHY)
	C2S(NL80211_CMD_GET_INTERFACE)
	C2S(NL80211_CMD_SET_INTERFACE)
	C2S(NL80211_CMD_NEW_INTERFACE)
	C2S(NL80211_CMD_DEL_INTERFACE)
	C2S(NL80211_CMD_GET_KEY)
	C2S(NL80211_CMD_SET_KEY)
	C2S(NL80211_CMD_NEW_KEY)
	C2S(NL80211_CMD_DEL_KEY)
	C2S(NL80211_CMD_GET_BEACON)
	C2S(NL80211_CMD_SET_BEACON)
	C2S(NL80211_CMD_START_AP)
	C2S(NL80211_CMD_STOP_AP)
	C2S(NL80211_CMD_GET_STATION)
	C2S(NL80211_CMD_SET_STATION)
	C2S(NL80211_CMD_NEW_STATION)
	C2S(NL80211_CMD_DEL_STATION)
	C2S(NL80211_CMD_GET_MPATH)
	C2S(NL80211_CMD_SET_MPATH)
	C2S(NL80211_CMD_NEW_MPATH)
	C2S(NL80211_CMD_DEL_MPATH)
	C2S(NL80211_CMD_SET_BSS)
	C2S(NL80211_CMD_SET_REG)
	C2S(NL80211_CMD_REQ_SET_REG)
	C2S(NL80211_CMD_GET_MESH_CONFIG)
	C2S(NL80211_CMD_SET_MESH_CONFIG)
	C2S(NL80211_CMD_SET_MGMT_EXTRA_IE)
	C2S(NL80211_CMD_GET_REG)
	C2S(NL80211_CMD_GET_SCAN)
	C2S(NL80211_CMD_TRIGGER_SCAN)
	C2S(NL80211_CMD_NEW_SCAN_RESULTS)
	C2S(NL80211_CMD_SCAN_ABORTED)
	C2S(NL80211_CMD_REG_CHANGE)
	C2S(NL80211_CMD_AUTHENTICATE)
	C2S(NL80211_CMD_ASSOCIATE)
	C2S(NL80211_CMD_DEAUTHENTICATE)
	C2S(NL80211_CMD_DISASSOCIATE)
	C2S(NL80211_CMD_MICHAEL_MIC_FAILURE)
	C2S(NL80211_CMD_REG_BEACON_HINT)
	C2S(NL80211_CMD_JOIN_IBSS)
	C2S(NL80211_CMD_LEAVE_IBSS)
	C2S(NL80211_CMD_TESTMODE)
	C2S(NL80211_CMD_CONNECT)
	C2S(NL80211_CMD_ROAM)
	C2S(NL80211_CMD_DISCONNECT)
	C2S(NL80211_CMD_SET_WIPHY_NETNS)
	C2S(NL80211_CMD_GET_SURVEY)
	C2S(NL80211_CMD_NEW_SURVEY_RESULTS)
	C2S(NL80211_CMD_SET_PMKSA)
	C2S(NL80211_CMD_DEL_PMKSA)
	C2S(NL80211_CMD_FLUSH_PMKSA)
	C2S(NL80211_CMD_REMAIN_ON_CHANNEL)
	C2S(NL80211_CMD_CANCEL_REMAIN_ON_CHANNEL)
	C2S(NL80211_CMD_SET_TX_BITRATE_MASK)
	C2S(NL80211_CMD_REGISTER_FRAME)
	C2S(NL80211_CMD_FRAME)
	C2S(NL80211_CMD_FRAME_TX_STATUS)
	C2S(NL80211_CMD_SET_POWER_SAVE)
	C2S(NL80211_CMD_GET_POWER_SAVE)
	C2S(NL80211_CMD_SET_CQM)
	C2S(NL80211_CMD_NOTIFY_CQM)
	C2S(NL80211_CMD_SET_CHANNEL)
	C2S(NL80211_CMD_SET_WDS_PEER)
	C2S(NL80211_CMD_FRAME_WAIT_CANCEL)
	C2S(NL80211_CMD_JOIN_MESH)
	C2S(NL80211_CMD_LEAVE_MESH)
	C2S(NL80211_CMD_UNPROT_DEAUTHENTICATE)
	C2S(NL80211_CMD_UNPROT_DISASSOCIATE)
	C2S(NL80211_CMD_NEW_PEER_CANDIDATE)
	C2S(NL80211_CMD_GET_WOWLAN)
	C2S(NL80211_CMD_SET_WOWLAN)
	C2S(NL80211_CMD_START_SCHED_SCAN)
	C2S(NL80211_CMD_STOP_SCHED_SCAN)
	C2S(NL80211_CMD_SCHED_SCAN_RESULTS)
	C2S(NL80211_CMD_SCHED_SCAN_STOPPED)
	C2S(NL80211_CMD_SET_REKEY_OFFLOAD)
	C2S(NL80211_CMD_PMKSA_CANDIDATE)
	C2S(NL80211_CMD_TDLS_OPER)
	C2S(NL80211_CMD_TDLS_MGMT)
	C2S(NL80211_CMD_UNEXPECTED_FRAME)
	C2S(NL80211_CMD_PROBE_CLIENT)
	C2S(NL80211_CMD_REGISTER_BEACONS)
	C2S(NL80211_CMD_UNEXPECTED_4ADDR_FRAME)
	C2S(NL80211_CMD_SET_NOACK_MAP)
	C2S(NL80211_CMD_CH_SWITCH_NOTIFY)
	C2S(NL80211_CMD_START_P2P_DEVICE)
	C2S(NL80211_CMD_STOP_P2P_DEVICE)
	C2S(NL80211_CMD_CONN_FAILED)
	C2S(NL80211_CMD_SET_MCAST_RATE)
	C2S(NL80211_CMD_SET_MAC_ACL)
	C2S(NL80211_CMD_RADAR_DETECT)
	C2S(NL80211_CMD_GET_PROTOCOL_FEATURES)
	C2S(NL80211_CMD_UPDATE_FT_IES)
	C2S(NL80211_CMD_FT_EVENT)
	C2S(NL80211_CMD_CRIT_PROTOCOL_START)
	C2S(NL80211_CMD_CRIT_PROTOCOL_STOP)
	C2S(NL80211_CMD_GET_COALESCE)
	C2S(NL80211_CMD_SET_COALESCE)
	C2S(NL80211_CMD_CHANNEL_SWITCH)
	C2S(NL80211_CMD_VENDOR)
	C2S(NL80211_CMD_SET_QOS_MAP)
	C2S(NL80211_CMD_ADD_TX_TS)
	C2S(NL80211_CMD_DEL_TX_TS)
	C2S(NL80211_CMD_GET_MPP)
	C2S(NL80211_CMD_JOIN_OCB)
	C2S(NL80211_CMD_LEAVE_OCB)
	C2S(NL80211_CMD_CH_SWITCH_STARTED_NOTIFY)
	C2S(NL80211_CMD_TDLS_CHANNEL_SWITCH)
	C2S(NL80211_CMD_TDLS_CANCEL_CHANNEL_SWITCH)
	C2S(NL80211_CMD_WIPHY_REG_CHANGE)
	C2S(NL80211_CMD_ABORT_SCAN)
	C2S(NL80211_CMD_START_NAN)
	C2S(NL80211_CMD_STOP_NAN)
	C2S(NL80211_CMD_ADD_NAN_FUNCTION)
	C2S(NL80211_CMD_DEL_NAN_FUNCTION)
	C2S(NL80211_CMD_CHANGE_NAN_CONFIG)
	C2S(NL80211_CMD_NAN_MATCH)
	C2S(NL80211_CMD_SET_MULTICAST_TO_UNICAST)
	C2S(NL80211_CMD_UPDATE_CONNECT_PARAMS)
	C2S(NL80211_CMD_SET_PMK)
	C2S(NL80211_CMD_DEL_PMK)
	C2S(NL80211_CMD_PORT_AUTHORIZED)
	C2S(NL80211_CMD_RELOAD_REGDB)
	C2S(NL80211_CMD_EXTERNAL_AUTH)
	C2S(NL80211_CMD_STA_OPMODE_CHANGED)
	C2S(NL80211_CMD_CONTROL_PORT_FRAME)
	C2S(NL80211_CMD_GET_FTM_RESPONDER_STATS)
	C2S(NL80211_CMD_PEER_MEASUREMENT_START)
	C2S(NL80211_CMD_PEER_MEASUREMENT_RESULT)
	C2S(NL80211_CMD_PEER_MEASUREMENT_COMPLETE)
	C2S(NL80211_CMD_NOTIFY_RADAR)
	C2S(NL80211_CMD_UPDATE_OWE_INFO)
	C2S(NL80211_CMD_PROBE_MESH_LINK)
	C2S(NL80211_CMD_SET_TID_CONFIG)
	C2S(NL80211_CMD_UNPROT_BEACON)
	C2S(NL80211_CMD_CONTROL_PORT_FRAME_TX_STATUS)
	C2S(NL80211_CMD_SET_SAR_SPECS)
	C2S(__NL80211_CMD_AFTER_LAST)
	}
#undef C2S

	return "NL80211_CMD_UNKNOWN";
}


static void mlme_event_auth(struct wpa_driver_nl80211_data *drv,
			    const u8 *frame, size_t len)
{
	const struct ieee80211_mgmt *mgmt;
	union wpa_event_data event;

	if (!(drv->capa.flags & WPA_DRIVER_FLAGS_SME) &&
	    drv->force_connect_cmd) {
		/*
		 * Avoid reporting two association events that would confuse
		 * the core code.
		 */
		wpa_printf(MSG_DEBUG,
			   "nl80211: Ignore auth event when using driver SME");
		return;
	}

	wpa_printf(MSG_DEBUG, "nl80211: Authenticate event");
	mgmt = (const struct ieee80211_mgmt *) frame;
	if (len < 24 + sizeof(mgmt->u.auth)) {
		wpa_printf(MSG_DEBUG, "nl80211: Too short association event "
			   "frame");
		return;
	}

	os_memcpy(drv->auth_bssid, mgmt->sa, ETH_ALEN);
	os_memset(drv->auth_attempt_bssid, 0, ETH_ALEN);
	os_memset(&event, 0, sizeof(event));
	os_memcpy(event.auth.peer, mgmt->sa, ETH_ALEN);
	event.auth.auth_type = le_to_host16(mgmt->u.auth.auth_alg);
	event.auth.auth_transaction =
		le_to_host16(mgmt->u.auth.auth_transaction);
	event.auth.status_code = le_to_host16(mgmt->u.auth.status_code);
	if (len > 24 + sizeof(mgmt->u.auth)) {
		event.auth.ies = mgmt->u.auth.variable;
		event.auth.ies_len = len - 24 - sizeof(mgmt->u.auth);
	}

	wpa_supplicant_event(drv->ctx, EVENT_AUTH, &event);
}


static void nl80211_parse_wmm_params(struct nlattr *wmm_attr,
				     struct wmm_params *wmm_params)
{
	struct nlattr *wmm_info[NL80211_STA_WME_MAX + 1];
	static struct nla_policy wme_policy[NL80211_STA_WME_MAX + 1] = {
		[NL80211_STA_WME_UAPSD_QUEUES] = { .type = NLA_U8 },
	};

	if (!wmm_attr ||
	    nla_parse_nested(wmm_info, NL80211_STA_WME_MAX, wmm_attr,
			     wme_policy) ||
	    !wmm_info[NL80211_STA_WME_UAPSD_QUEUES])
		return;

	wmm_params->uapsd_queues =
		nla_get_u8(wmm_info[NL80211_STA_WME_UAPSD_QUEUES]);
	wmm_params->info_bitmap |= WMM_PARAMS_UAPSD_QUEUES_INFO;
}


static void mlme_event_assoc(struct wpa_driver_nl80211_data *drv,
			     const u8 *frame, size_t len, struct nlattr *wmm,
			     struct nlattr *req_ie)
{
	const struct ieee80211_mgmt *mgmt;
	union wpa_event_data event;
	u16 status;
	int ssid_len;

	if (!(drv->capa.flags & WPA_DRIVER_FLAGS_SME) &&
	    drv->force_connect_cmd) {
		/*
		 * Avoid reporting two association events that would confuse
		 * the core code.
		 */
		wpa_printf(MSG_DEBUG,
			   "nl80211: Ignore assoc event when using driver SME");
		return;
	}

	wpa_printf(MSG_DEBUG, "nl80211: Associate event");
	mgmt = (const struct ieee80211_mgmt *) frame;
	if (len < 24 + sizeof(mgmt->u.assoc_resp)) {
		wpa_printf(MSG_DEBUG, "nl80211: Too short association event "
			   "frame");
		return;
	}

	status = le_to_host16(mgmt->u.assoc_resp.status_code);
	if (status != WLAN_STATUS_SUCCESS) {
		os_memset(&event, 0, sizeof(event));
		event.assoc_reject.bssid = mgmt->bssid;
		if (len > 24 + sizeof(mgmt->u.assoc_resp)) {
			event.assoc_reject.resp_ies =
				(u8 *) mgmt->u.assoc_resp.variable;
			event.assoc_reject.resp_ies_len =
				len - 24 - sizeof(mgmt->u.assoc_resp);
		}
		event.assoc_reject.status_code = status;

		wpa_supplicant_event(drv->ctx, EVENT_ASSOC_REJECT, &event);
		return;
	}

	drv->associated = 1;
	os_memcpy(drv->bssid, mgmt->sa, ETH_ALEN);
	os_memcpy(drv->prev_bssid, mgmt->sa, ETH_ALEN);

	os_memset(&event, 0, sizeof(event));
	event.assoc_info.resp_frame = frame;
	event.assoc_info.resp_frame_len = len;
	if (len > 24 + sizeof(mgmt->u.assoc_resp)) {
		event.assoc_info.resp_ies = (u8 *) mgmt->u.assoc_resp.variable;
		event.assoc_info.resp_ies_len =
			len - 24 - sizeof(mgmt->u.assoc_resp);
	}

	if (req_ie) {
		event.assoc_info.req_ies = nla_data(req_ie);
		event.assoc_info.req_ies_len = nla_len(req_ie);
	}

	/* When this association was initiated outside of wpa_supplicant,
	 * drv->ssid needs to be set here to satisfy later checking. */
#ifdef CFG_INTERFACE
	//TODO: Currently it is not supported
#else
	ssid_len = nl80211_get_assoc_ssid(drv, drv->ssid);
	if (ssid_len > 0) {
		drv->ssid_len = ssid_len;
		wpa_printf(MSG_DEBUG,
			   "nl80211: Set drv->ssid based on scan res info to '%s'",
			   wpa_ssid_txt(drv->ssid, drv->ssid_len));
	}
#endif
	event.assoc_info.freq = drv->assoc_freq;
	drv->first_bss->freq = drv->assoc_freq;

	nl80211_parse_wmm_params(wmm, &event.assoc_info.wmm_params);

	wpa_supplicant_event(drv->ctx, EVENT_ASSOC, &event);
}


#ifdef CONFIG_DRIVER_NL80211_QCA

static int qca_drv_connect_fail_reason_code_handler(struct nl_msg *msg,
						    void *arg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *tb_sta_info[QCA_WLAN_VENDOR_ATTR_GET_STA_INFO_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	u32 *reason_code = arg;

	*reason_code = 0;
	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_VENDOR_DATA]) {
		wpa_printf(MSG_ERROR, "%s: Vendor data not found", __func__);
		return NL_SKIP;
	}

	nla_parse(tb_sta_info, QCA_WLAN_VENDOR_ATTR_GET_STA_INFO_MAX,
		  nla_data(tb[NL80211_ATTR_VENDOR_DATA]),
		  nla_len(tb[NL80211_ATTR_VENDOR_DATA]), NULL);

	if (!tb_sta_info[QCA_WLAN_VENDOR_ATTR_GET_STA_INFO_CONNECT_FAIL_REASON_CODE]) {
		wpa_printf(MSG_INFO, "%s: Vendor attr not found", __func__);
		return NL_SKIP;
	}

	*reason_code = nla_get_u32(tb_sta_info[QCA_WLAN_VENDOR_ATTR_GET_STA_INFO_CONNECT_FAIL_REASON_CODE]);

	return NL_SKIP;
}


static enum qca_sta_connect_fail_reason_codes
drv_get_connect_fail_reason_code(struct wpa_driver_nl80211_data *drv)
{
	enum qca_sta_connect_fail_reason_codes reason_code;
	struct nl_msg *msg;
	int ret;

	msg = nl80211_drv_msg(drv, 0, NL80211_CMD_VENDOR);
	if (!msg || nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, OUI_QCA) ||
	    nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD,
			QCA_NL80211_VENDOR_SUBCMD_GET_STA_INFO)) {
		nlmsg_free(msg);
		return 0;
	}

	ret = send_and_recv_msgs(drv, msg,
				 qca_drv_connect_fail_reason_code_handler,
				 &reason_code, NULL, NULL);
	if (ret)
		wpa_printf(MSG_DEBUG,
			   "nl80211: Get connect fail reason_code failed: ret=%d (%s)",
			   ret, strerror(-ret));

	return reason_code;
}


static enum sta_connect_fail_reason_codes
convert_connect_fail_reason_codes(enum qca_sta_connect_fail_reason_codes
				  reason_code)
{
	switch (reason_code) {
	case QCA_STA_CONNECT_FAIL_REASON_NO_BSS_FOUND:
		return STA_CONNECT_FAIL_REASON_NO_BSS_FOUND;
	case QCA_STA_CONNECT_FAIL_REASON_AUTH_TX_FAIL:
		return STA_CONNECT_FAIL_REASON_AUTH_TX_FAIL;
	case QCA_STA_CONNECT_FAIL_REASON_AUTH_NO_ACK_RECEIVED:
		return STA_CONNECT_FAIL_REASON_AUTH_NO_ACK_RECEIVED;
	case QCA_STA_CONNECT_FAIL_REASON_AUTH_NO_RESP_RECEIVED:
		return STA_CONNECT_FAIL_REASON_AUTH_NO_RESP_RECEIVED;
	case QCA_STA_CONNECT_FAIL_REASON_ASSOC_REQ_TX_FAIL:
		return STA_CONNECT_FAIL_REASON_ASSOC_REQ_TX_FAIL;
	case QCA_STA_CONNECT_FAIL_REASON_ASSOC_NO_ACK_RECEIVED:
		return STA_CONNECT_FAIL_REASON_ASSOC_NO_ACK_RECEIVED;
	case QCA_STA_CONNECT_FAIL_REASON_ASSOC_NO_RESP_RECEIVED:
		return STA_CONNECT_FAIL_REASON_ASSOC_NO_RESP_RECEIVED;
	default:
		return STA_CONNECT_FAIL_REASON_UNSPECIFIED;
	}
}

#endif /* CONFIG_DRIVER_NL80211_QCA */


static void mlme_event_connect(struct wpa_driver_nl80211_data *drv,
			       enum nl80211_commands cmd, struct nlattr *status,
			       struct nlattr *addr, struct nlattr *req_ie,
			       struct nlattr *resp_ie,
			       struct nlattr *timed_out,
			       struct nlattr *timeout_reason,
			       struct nlattr *authorized,
			       struct nlattr *key_replay_ctr,
			       struct nlattr *ptk_kck,
			       struct nlattr *ptk_kek,
			       struct nlattr *subnet_status,
			       struct nlattr *fils_erp_next_seq_num,
			       struct nlattr *fils_pmk,
			       struct nlattr *fils_pmkid)
{
	union wpa_event_data event;
	const u8 *ssid = NULL;
	u16 status_code;
	int ssid_len;

	if (drv->capa.flags & WPA_DRIVER_FLAGS_SME) {
		/*
		 * Avoid reporting two association events that would confuse
		 * the core code.
		 */
		wpa_printf(MSG_DEBUG, "nl80211: Ignore connect event (cmd=%d) "
			   "when using userspace SME", cmd);
		return;
	}

	drv->connect_reassoc = 0;

	status_code = status ? nla_get_u16(status) : WLAN_STATUS_SUCCESS;

	if (cmd == NL80211_CMD_CONNECT) {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Connect event (status=%u ignore_next_local_disconnect=%d)",
			   status_code, drv->ignore_next_local_disconnect);
	} else if (cmd == NL80211_CMD_ROAM) {
		wpa_printf(MSG_DEBUG, "nl80211: Roam event");
	}

	os_memset(&event, 0, sizeof(event));
	if (cmd == NL80211_CMD_CONNECT && status_code != WLAN_STATUS_SUCCESS) {
		if (addr)
			event.assoc_reject.bssid = nla_data(addr);
		if (drv->ignore_next_local_disconnect) {
			drv->ignore_next_local_disconnect = 0;
			if (!event.assoc_reject.bssid ||
			    (os_memcmp(event.assoc_reject.bssid,
				       drv->auth_attempt_bssid,
				       ETH_ALEN) != 0)) {
				/*
				 * Ignore the event that came without a BSSID or
				 * for the old connection since this is likely
				 * not relevant to the new Connect command.
				 */
				wpa_printf(MSG_DEBUG,
					   "nl80211: Ignore connection failure event triggered during reassociation");
				return;
			}
		}
		if (resp_ie) {
			event.assoc_reject.resp_ies = nla_data(resp_ie);
			event.assoc_reject.resp_ies_len = nla_len(resp_ie);
		}
		event.assoc_reject.status_code = status_code;
		event.assoc_reject.timed_out = timed_out != NULL;
		if (timed_out && timeout_reason) {
			enum nl80211_timeout_reason reason;

			reason = nla_get_u32(timeout_reason);
			switch (reason) {
			case NL80211_TIMEOUT_SCAN:
				event.assoc_reject.timeout_reason = "scan";
				break;
			case NL80211_TIMEOUT_AUTH:
				event.assoc_reject.timeout_reason = "auth";
				break;
			case NL80211_TIMEOUT_ASSOC:
				event.assoc_reject.timeout_reason = "assoc";
				break;
			default:
				break;
			}
		}
		if (fils_erp_next_seq_num)
			event.assoc_reject.fils_erp_next_seq_num =
				nla_get_u16(fils_erp_next_seq_num);

#ifdef CONFIG_DRIVER_NL80211_QCA
		if (drv->get_sta_info_vendor_cmd_avail) {
			enum qca_sta_connect_fail_reason_codes reason_code;

			reason_code = drv_get_connect_fail_reason_code(drv);
			event.assoc_reject.reason_code =
				convert_connect_fail_reason_codes(reason_code);
		}
#endif /* CONFIG_DRIVER_NL80211_QCA */

		wpa_supplicant_event(drv->ctx, EVENT_ASSOC_REJECT, &event);
		return;
	}

	drv->associated = 1;
	if (addr) {
		os_memcpy(drv->bssid, nla_data(addr), ETH_ALEN);
		os_memcpy(drv->prev_bssid, drv->bssid, ETH_ALEN);
	}

	if (req_ie) {
		event.assoc_info.req_ies = nla_data(req_ie);
		event.assoc_info.req_ies_len = nla_len(req_ie);

		if (cmd == NL80211_CMD_ROAM) {
			ssid = get_ie(event.assoc_info.req_ies,
				      event.assoc_info.req_ies_len,
				      WLAN_EID_SSID);
			if (ssid && ssid[1] > 0 && ssid[1] <= 32) {
				drv->ssid_len = ssid[1];
				os_memcpy(drv->ssid, ssid + 2, ssid[1]);
				wpa_printf(MSG_DEBUG,
					   "nl80211: Set drv->ssid based on req_ie to '%s'",
					   wpa_ssid_txt(drv->ssid,
							drv->ssid_len));
			}
		}
	}
	if (resp_ie) {
		event.assoc_info.resp_ies = nla_data(resp_ie);
		event.assoc_info.resp_ies_len = nla_len(resp_ie);
	}

	event.assoc_info.freq = nl80211_get_assoc_freq(drv);
	drv->first_bss->freq = drv->assoc_freq;

	if ((!ssid || ssid[1] == 0 || ssid[1] > 32) &&
	    (ssid_len = nl80211_get_assoc_ssid(drv, drv->ssid)) > 0) {
		/* When this connection was initiated outside of wpa_supplicant,
		 * drv->ssid needs to be set here to satisfy later checking. */
		drv->ssid_len = ssid_len;
		wpa_printf(MSG_DEBUG,
			   "nl80211: Set drv->ssid based on scan res info to '%s'",
			   wpa_ssid_txt(drv->ssid, drv->ssid_len));
	}

	if (authorized && nla_get_u8(authorized)) {
		event.assoc_info.authorized = 1;
		wpa_printf(MSG_DEBUG, "nl80211: connection authorized");
	}
	if (key_replay_ctr) {
		event.assoc_info.key_replay_ctr = nla_data(key_replay_ctr);
		event.assoc_info.key_replay_ctr_len = nla_len(key_replay_ctr);
	}
	if (ptk_kck) {
		event.assoc_info.ptk_kck = nla_data(ptk_kck);
		event.assoc_info.ptk_kck_len = nla_len(ptk_kck);
	}
	if (ptk_kek) {
		event.assoc_info.ptk_kek = nla_data(ptk_kek);
		event.assoc_info.ptk_kek_len = nla_len(ptk_kek);
	}

	if (subnet_status) {
		/*
		 * At least for now, this is only available from
		 * QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_SUBNET_STATUS and that
		 * attribute has the same values 0, 1, 2 as are used in the
		 * variable here, so no mapping between different values are
		 * needed.
		 */
		event.assoc_info.subnet_status = nla_get_u8(subnet_status);
	}

	if (fils_erp_next_seq_num)
		event.assoc_info.fils_erp_next_seq_num =
			nla_get_u16(fils_erp_next_seq_num);

	if (fils_pmk) {
		event.assoc_info.fils_pmk = nla_data(fils_pmk);
		event.assoc_info.fils_pmk_len = nla_len(fils_pmk);
	}

	if (fils_pmkid)
		event.assoc_info.fils_pmkid = nla_data(fils_pmkid);

	wpa_supplicant_event(drv->ctx, EVENT_ASSOC, &event);

	/* Avoid a race condition by stopping to ignore any following
	 * disconnection events now that the driver has indicated it is
	 * connected since that connection could have been triggered by a roam
	 * operation that happened in parallel with the disconnection request.
	 */
	drv->ignore_next_local_disconnect = 0;
}


static void mlme_event_disconnect(struct wpa_driver_nl80211_data *drv,
				  struct nlattr *reason, struct nlattr *addr,
				  struct nlattr *by_ap)
{
	union wpa_event_data data;
	unsigned int locally_generated = by_ap == NULL;

	if (drv->capa.flags & WPA_DRIVER_FLAGS_SME) {
		/*
		 * Avoid reporting two disassociation events that could
		 * confuse the core code.
		 */
		wpa_printf(MSG_DEBUG, "nl80211: Ignore disconnect "
			   "event when using userspace SME");
		return;
	}

	if (drv->ignore_next_local_disconnect) {
		drv->ignore_next_local_disconnect = 0;
		if (locally_generated) {
			wpa_printf(MSG_DEBUG, "nl80211: Ignore disconnect "
				   "event triggered during reassociation");
			return;
		}
		wpa_printf(MSG_WARNING, "nl80211: Was expecting local "
			   "disconnect but got another disconnect "
			   "event first");
	}

	wpa_printf(MSG_DEBUG, "nl80211: Disconnect event");
	nl80211_mark_disconnected(drv);
	os_memset(&data, 0, sizeof(data));
	if (reason)
		data.deauth_info.reason_code = nla_get_u16(reason);
	data.deauth_info.locally_generated = by_ap == NULL;
	wpa_supplicant_event(drv->ctx, EVENT_DEAUTH, &data);
}


static int calculate_chan_offset(int width, int freq, int cf1, int cf2)
{
	int freq1 = 0;

	switch (convert2width(width)) {
	case CHAN_WIDTH_20_NOHT:
	case CHAN_WIDTH_20:
		return 0;
	case CHAN_WIDTH_40:
		freq1 = cf1 - 10;
		break;
	case CHAN_WIDTH_80:
		freq1 = cf1 - 30;
		break;
	case CHAN_WIDTH_160:
		freq1 = cf1 - 70;
		break;
	case CHAN_WIDTH_80P80:
		freq1 = cf1 - 30;
		break;
	case CHAN_WIDTH_UNKNOWN:
	case CHAN_WIDTH_2160:
	case CHAN_WIDTH_4320:
	case CHAN_WIDTH_6480:
	case CHAN_WIDTH_8640:
		/* FIXME: implement this */
		return 0;
	}

	return (abs(freq - freq1) / 20) % 2 == 0 ? 1 : -1;
}


static void mlme_event_ch_switch(struct wpa_driver_nl80211_data *drv,
				 struct nlattr *ifindex, struct nlattr *freq,
				 struct nlattr *type, struct nlattr *bw,
				 struct nlattr *cf1, struct nlattr *cf2,
				 int finished)
{
	struct i802_bss *bss;
	union wpa_event_data data;
	int ht_enabled = 1;
	int chan_offset = 0;
	int ifidx;

	wpa_printf(MSG_DEBUG, "nl80211: Channel switch%s event",
		   finished ? "" : " started");

	if (!freq)
		return;

	ifidx = nla_get_u32(ifindex);
	bss = get_bss_ifindex(drv, ifidx);
	if (bss == NULL) {
		wpa_printf(MSG_WARNING, "nl80211: Unknown ifindex (%d) for channel switch, ignoring",
			   ifidx);
		return;
	}

	if (type) {
		enum nl80211_channel_type ch_type = nla_get_u32(type);

		wpa_printf(MSG_DEBUG, "nl80211: Channel type: %d", ch_type);
		switch (ch_type) {
		case NL80211_CHAN_NO_HT:
			ht_enabled = 0;
			break;
		case NL80211_CHAN_HT20:
			break;
		case NL80211_CHAN_HT40PLUS:
			chan_offset = 1;
			break;
		case NL80211_CHAN_HT40MINUS:
			chan_offset = -1;
			break;
		}
	} else if (bw && cf1) {
		/* This can happen for example with VHT80 ch switch */
		chan_offset = calculate_chan_offset(nla_get_u32(bw),
						    nla_get_u32(freq),
						    nla_get_u32(cf1),
						    cf2 ? nla_get_u32(cf2) : 0);
		wpa_printf(MSG_DEBUG, "nl80211: Calculated channel offset: %d",
			   chan_offset);
	} else {
		wpa_printf(MSG_WARNING, "nl80211: Unknown secondary channel information - following channel definition calculations may fail");
	}

	os_memset(&data, 0, sizeof(data));
	data.ch_switch.freq = nla_get_u32(freq);
	data.ch_switch.ht_enabled = ht_enabled;
	data.ch_switch.ch_offset = chan_offset;
	if (bw)
		data.ch_switch.ch_width = convert2width(nla_get_u32(bw));
	if (cf1)
		data.ch_switch.cf1 = nla_get_u32(cf1);
	if (cf2)
		data.ch_switch.cf2 = nla_get_u32(cf2);

	if (finished)
		bss->freq = data.ch_switch.freq;
	drv->assoc_freq = data.ch_switch.freq;

	wpa_supplicant_event(bss->ctx, finished ?
			     EVENT_CH_SWITCH : EVENT_CH_SWITCH_STARTED, &data);
}


static void mlme_timeout_event(struct wpa_driver_nl80211_data *drv,
			       enum nl80211_commands cmd, struct nlattr *addr)
{
	union wpa_event_data event;
	enum wpa_event_type ev;
#if 0
#ifdef CFG_INTERFACE
	if (sizeof(addr) != ETH_ALEN)
		return;

	wpa_printf(MSG_DEBUG, "nl80211: MLME event %d; timeout with " MACSTR,
		   cmd, MAC2STR((u8 *)(addr)));
#else
#endif
#endif
	if (nla_len(addr) != ETH_ALEN)
		return;

	wpa_printf(MSG_DEBUG, "nl80211: MLME event %d; timeout with " MACSTR,
		   cmd, MAC2STR((u8 *) nla_data(addr)));

	if (cmd == NL80211_CMD_AUTHENTICATE)
		ev = EVENT_AUTH_TIMED_OUT;
	else if (cmd == NL80211_CMD_ASSOCIATE)
		ev = EVENT_ASSOC_TIMED_OUT;
	else
		return;

	os_memset(&event, 0, sizeof(event));
	os_memcpy(event.timeout_event.addr, nla_data(addr), ETH_ALEN);
	wpa_supplicant_event(drv->ctx, ev, &event);
}
//DEEPAK
#ifdef CFG_INTERFACE
static void cfg_mlme_event_mgmt(struct i802_bss *bss,
			    struct nrf_wifi_umac_event_mlme *devent, size_t len)
{
	struct wpa_driver_nl80211_data *drv = bss->drv;
	const struct ieee80211_mgmt *mgmt;

	union wpa_event_data event;
	u16 fc, stype;
	int ssi_signal = 0;
	int rx_freq = 0;

	wpa_printf(MSG_MSGDUMP, "nl80211: Frame event");
	mgmt = (const struct ieee80211_mgmt *)devent->frame.frame;
	if (len < 24) {
		wpa_printf(MSG_DEBUG, "nl80211: Too short management frame");
		return;
	}

	fc = le_to_host16(mgmt->frame_control);
	stype = WLAN_FC_GET_STYPE(fc);

	os_memset(&event, 0, sizeof(event));

	if (devent->frequency) {
		event.rx_mgmt.freq = devent->frequency;
		rx_freq = drv->last_mgmt_freq = event.rx_mgmt.freq;
	}
	wpa_printf(MSG_DEBUG,
		   "nl80211: RX frame da=" MACSTR " sa=" MACSTR " bssid=" MACSTR
		   " freq=%d ssi_signal=%d fc=0x%x seq_ctrl=0x%x stype=%u (%s) len=%u",
		   MAC2STR(mgmt->da), MAC2STR(mgmt->sa), MAC2STR(mgmt->bssid),
		   rx_freq, devent->rx_signal_dbm, fc,
		   le_to_host16(mgmt->seq_ctrl), stype, fc2str(fc),
		   (unsigned int) len);
	event.rx_mgmt.frame = devent->frame.frame;
	event.rx_mgmt.frame_len = devent->frame.frame_len;
	event.rx_mgmt.ssi_signal = devent->rx_signal_dbm;
	event.rx_mgmt.drv_priv = bss;
	wpa_supplicant_event(drv->ctx, EVENT_RX_MGMT, &event);
}
#endif
static void mlme_event_mgmt(struct i802_bss *bss,
			    struct nlattr *freq, struct nlattr *sig,
			    const u8 *frame, size_t len)
{
	struct wpa_driver_nl80211_data *drv = bss->drv;
	const struct ieee80211_mgmt *mgmt;
	union wpa_event_data event;
	u16 fc, stype;
	int ssi_signal = 0;
	int rx_freq = 0;

	wpa_printf(MSG_MSGDUMP, "nl80211: Frame event");
	mgmt = (const struct ieee80211_mgmt *) frame;
	if (len < 24) {
		wpa_printf(MSG_DEBUG, "nl80211: Too short management frame");
		return;
	}

	fc = le_to_host16(mgmt->frame_control);
	stype = WLAN_FC_GET_STYPE(fc);

	if (sig)
		ssi_signal = (s32) nla_get_u32(sig);

	os_memset(&event, 0, sizeof(event));
	if (freq) {
		event.rx_mgmt.freq = nla_get_u32(freq);
		rx_freq = drv->last_mgmt_freq = event.rx_mgmt.freq;
	}
	wpa_printf(MSG_DEBUG,
		   "nl80211: RX frame da=" MACSTR " sa=" MACSTR " bssid=" MACSTR
		   " freq=%d ssi_signal=%d fc=0x%x seq_ctrl=0x%x stype=%u (%s) len=%u",
		   MAC2STR(mgmt->da), MAC2STR(mgmt->sa), MAC2STR(mgmt->bssid),
		   rx_freq, ssi_signal, fc,
		   le_to_host16(mgmt->seq_ctrl), stype, fc2str(fc),
		   (unsigned int) len);
	event.rx_mgmt.frame = frame;
	event.rx_mgmt.frame_len = len;
	event.rx_mgmt.ssi_signal = ssi_signal;
	event.rx_mgmt.drv_priv = bss;
	wpa_supplicant_event(drv->ctx, EVENT_RX_MGMT, &event);
}

#ifdef CFG_INTERFACE
static void cfg_mlme_event_mgmt_tx_status(struct wpa_driver_nl80211_data *drv,
				      struct nrf_wifi_umac_event_mlme *mlme_event,
				      size_t len)
{
	union wpa_event_data event;
	const struct ieee80211_hdr *hdr;
	u16 fc;
	u64 cookie_val;
	int ack = mlme_event->nrf_wifi_flags & NRF_WIFI_EVENT_MLME_ACK ? 1 : 0;
	unsigned long long host_cookie = 0;
	struct cookie_info *cookie_info = NULL;
	struct cookie_info *tmp = NULL;
	int match_cookie_found = 0;

	wpa_printf(MSG_DEBUG, "nl80211: Frame TX status event");
	if (!is_ap_interface(drv->nlmode)) {
		
		//if (!mlme_event->valid_fields & NRF_WIFI_EVENT_MLME_COOKIE_VALID)
		//	return;

		if (dl_list_empty(&drv->global->cookie_list))
                	return;

	        dl_list_for_each_safe(cookie_info, tmp, &drv->global->cookie_list,
                              struct cookie_info, list) {
			if (cookie_info) {
				if (cookie_info->rpu_cookie != mlme_event->cookie)
                                	continue;
	                        host_cookie = cookie_info->host_cookie;
        	                match_cookie_found = 1;
				dl_list_del(&cookie_info->list);
                        	os_free(cookie_info);
				break;
			}
		}	
                
		if (match_cookie_found)
			cookie_val = host_cookie;
		wpa_printf(MSG_DEBUG, "nl80211: Action TX status:"
			   " cookie=0x%llx%s (ack=%d)",
			   (long long unsigned int) cookie_val,
			   cookie_val == drv->send_frame_cookie ?
			   " (match)" : " (unknown)", ack != 0);
		if (cookie_val != drv->send_frame_cookie)
			return;
	}

	hdr = (const struct ieee80211_hdr *) mlme_event->frame.frame;
	fc = le_to_host16(hdr->frame_control);

	os_memset(&event, 0, sizeof(event));
	event.tx_status.type = WLAN_FC_GET_TYPE(fc);
	event.tx_status.stype = WLAN_FC_GET_STYPE(fc);
	event.tx_status.dst = hdr->addr1;
	event.tx_status.data = mlme_event->frame.frame;
	event.tx_status.data_len = mlme_event->frame.frame_len;
	event.tx_status.ack = ack != 0;
	wpa_supplicant_event(drv->ctx, EVENT_TX_STATUS, &event);
}
#endif

static void mlme_event_mgmt_tx_status(struct wpa_driver_nl80211_data *drv,
				      struct nlattr *cookie, const u8 *frame,
				      size_t len, struct nlattr *ack)
{
	union wpa_event_data event;
	const struct ieee80211_hdr *hdr = (const struct ieee80211_hdr *) frame;
	u16 fc = le_to_host16(hdr->frame_control);
	u64 cookie_val = 0;

	if (cookie)
		cookie_val = nla_get_u64(cookie);
	wpa_printf(MSG_DEBUG,
		   "nl80211: Frame TX status event A1=" MACSTR
		   " %sstype=%d cookie=0x%llx%s ack=%d",
		   MAC2STR(hdr->addr1),
		   WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_MGMT ? "not-mgmt " : "",
		   WLAN_FC_GET_STYPE(fc), (long long unsigned int) cookie_val,
		   cookie ? "" : "(N/A)", ack != NULL);

	if (cookie_val && cookie_val == drv->eapol_tx_cookie &&
	    len >= ETH_HLEN &&
	    WPA_GET_BE16(frame + 2 * ETH_ALEN) == ETH_P_PAE) {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Work around misdelivered control port TX status for EAPOL");
		nl80211_control_port_frame_tx_status(drv, frame, len, ack,
						     cookie);
		return;
	}

	if (WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_MGMT)
		return;

	if (!is_ap_interface(drv->nlmode) &&
	    WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_ACTION) {
		if (!cookie)
			return;

		wpa_printf(MSG_DEBUG,
			   "nl80211: Frame TX status: cookie=0x%llx%s (ack=%d)",
			   (long long unsigned int) cookie_val,
			   cookie_val == drv->send_frame_cookie ?
			   " (match)" : " (unknown)", ack != NULL);
		if (cookie_val != drv->send_frame_cookie)
			return;
	} else if (!is_ap_interface(drv->nlmode) &&
		   WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_AUTH) {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Authentication frame TX status: ack=%d",
			   !!ack);
	}

	os_memset(&event, 0, sizeof(event));
	event.tx_status.type = WLAN_FC_GET_TYPE(fc);
	event.tx_status.stype = WLAN_FC_GET_STYPE(fc);
	event.tx_status.dst = hdr->addr1;
	event.tx_status.data = frame;
	event.tx_status.data_len = len;
	event.tx_status.ack = ack != NULL;
	wpa_supplicant_event(drv->ctx, EVENT_TX_STATUS, &event);
}


static void mlme_event_deauth_disassoc(struct wpa_driver_nl80211_data *drv,
				       enum wpa_event_type type,
				       const u8 *frame, size_t len)
{
	const struct ieee80211_mgmt *mgmt;
	union wpa_event_data event;
	const u8 *bssid = NULL;
	u16 reason_code = 0;

	if (type == EVENT_DEAUTH)
		wpa_printf(MSG_DEBUG, "nl80211: Deauthenticate event");
	else
		wpa_printf(MSG_DEBUG, "nl80211: Disassociate event");

	mgmt = (const struct ieee80211_mgmt *) frame;
	if (len >= 24) {
		bssid = mgmt->bssid;

		if ((drv->capa.flags & WPA_DRIVER_FLAGS_SME) &&
		    !drv->associated &&
		    os_memcmp(bssid, drv->auth_bssid, ETH_ALEN) != 0 &&
		    os_memcmp(bssid, drv->auth_attempt_bssid, ETH_ALEN) != 0 &&
		    os_memcmp(bssid, drv->prev_bssid, ETH_ALEN) == 0) {
			/*
			 * Avoid issues with some roaming cases where
			 * disconnection event for the old AP may show up after
			 * we have started connection with the new AP.
			 * In case of locally generated event clear
			 * ignore_next_local_deauth as well, to avoid next local
			 * deauth event be wrongly ignored.
			 */
			if (!os_memcmp(mgmt->sa, drv->first_bss->addr,
				       ETH_ALEN)) {
				wpa_printf(MSG_DEBUG,
					   "nl80211: Received a locally generated deauth event. Clear ignore_next_local_deauth flag");
				drv->ignore_next_local_deauth = 0;
			} else {
				wpa_printf(MSG_DEBUG,
					   "nl80211: Ignore deauth/disassoc event from old AP " MACSTR " when already authenticating with " MACSTR,
					   MAC2STR(bssid),
					   MAC2STR(drv->auth_attempt_bssid));
			}
			return;
		}

		if (!(drv->capa.flags & WPA_DRIVER_FLAGS_SME) &&
		    drv->connect_reassoc && drv->associated &&
		    os_memcmp(bssid, drv->prev_bssid, ETH_ALEN) == 0 &&
		    os_memcmp(bssid, drv->auth_attempt_bssid, ETH_ALEN) != 0) {
			/*
			 * Avoid issues with some roaming cases where
			 * disconnection event for the old AP may show up after
			 * we have started connection with the new AP.
			 */
			wpa_printf(MSG_DEBUG,
				   "nl80211: Ignore deauth/disassoc event from old AP "
				   MACSTR
				   " when already connecting with " MACSTR,
				   MAC2STR(bssid),
				   MAC2STR(drv->auth_attempt_bssid));
			return;
		}

		if (drv->associated != 0 &&
		    os_memcmp(bssid, drv->bssid, ETH_ALEN) != 0 &&
		    os_memcmp(bssid, drv->auth_bssid, ETH_ALEN) != 0) {
			/*
			 * We have presumably received this deauth as a
			 * response to a clear_state_mismatch() outgoing
			 * deauth.  Don't let it take us offline!
			 */
			wpa_printf(MSG_DEBUG, "nl80211: Deauth received "
				   "from Unknown BSSID " MACSTR " -- ignoring",
				   MAC2STR(bssid));
			return;
		}
	}

	nl80211_mark_disconnected(drv);
	os_memset(&event, 0, sizeof(event));

	/* Note: Same offset for Reason Code in both frame subtypes */
	if (len >= 24 + sizeof(mgmt->u.deauth))
		reason_code = le_to_host16(mgmt->u.deauth.reason_code);

	if (type == EVENT_DISASSOC) {
		event.disassoc_info.locally_generated =
			!os_memcmp(mgmt->sa, drv->first_bss->addr, ETH_ALEN);
		event.disassoc_info.addr = bssid;
		event.disassoc_info.reason_code = reason_code;
		if (frame + len > mgmt->u.disassoc.variable) {
			event.disassoc_info.ie = mgmt->u.disassoc.variable;
			event.disassoc_info.ie_len = frame + len -
				mgmt->u.disassoc.variable;
		}
	} else {
		event.deauth_info.locally_generated =
			!os_memcmp(mgmt->sa, drv->first_bss->addr, ETH_ALEN);
		if (drv->ignore_deauth_event) {
			wpa_printf(MSG_DEBUG, "nl80211: Ignore deauth event due to previous forced deauth-during-auth");
			drv->ignore_deauth_event = 0;
			if (event.deauth_info.locally_generated)
				drv->ignore_next_local_deauth = 0;
			return;
		}
		if (drv->ignore_next_local_deauth) {
			drv->ignore_next_local_deauth = 0;
			if (event.deauth_info.locally_generated) {
				wpa_printf(MSG_DEBUG, "nl80211: Ignore deauth event triggered due to own deauth request");
				return;
			}
			wpa_printf(MSG_WARNING, "nl80211: Was expecting local deauth but got another disconnect event first");
		}
		event.deauth_info.addr = bssid;
		event.deauth_info.reason_code = reason_code;
		if (frame + len > mgmt->u.deauth.variable) {
			event.deauth_info.ie = mgmt->u.deauth.variable;
			event.deauth_info.ie_len = frame + len -
				mgmt->u.deauth.variable;
		}
	}

	wpa_supplicant_event(drv->ctx, type, &event);
}


static void mlme_event_unprot_disconnect(struct wpa_driver_nl80211_data *drv,
					 enum wpa_event_type type,
					 const u8 *frame, size_t len)
{
	const struct ieee80211_mgmt *mgmt;
	union wpa_event_data event;
	u16 reason_code = 0;

	if (type == EVENT_UNPROT_DEAUTH)
		wpa_printf(MSG_DEBUG, "nl80211: Unprot Deauthenticate event");
	else
		wpa_printf(MSG_DEBUG, "nl80211: Unprot Disassociate event");

	if (len < 24)
		return;

	mgmt = (const struct ieee80211_mgmt *) frame;

	os_memset(&event, 0, sizeof(event));
	/* Note: Same offset for Reason Code in both frame subtypes */
	if (len >= 24 + sizeof(mgmt->u.deauth))
		reason_code = le_to_host16(mgmt->u.deauth.reason_code);

	if (type == EVENT_UNPROT_DISASSOC) {
		event.unprot_disassoc.sa = mgmt->sa;
		event.unprot_disassoc.da = mgmt->da;
		event.unprot_disassoc.reason_code = reason_code;
	} else {
		event.unprot_deauth.sa = mgmt->sa;
		event.unprot_deauth.da = mgmt->da;
		event.unprot_deauth.reason_code = reason_code;
	}

	wpa_supplicant_event(drv->ctx, type, &event);
}


static void mlme_event_unprot_beacon(struct wpa_driver_nl80211_data *drv,
				     const u8 *frame, size_t len)
{
	const struct ieee80211_mgmt *mgmt;
	union wpa_event_data event;

	if (len < 24)
		return;

	mgmt = (const struct ieee80211_mgmt *) frame;

	os_memset(&event, 0, sizeof(event));
	event.unprot_beacon.sa = mgmt->sa;
	wpa_supplicant_event(drv->ctx, EVENT_UNPROT_BEACON, &event);
}


#ifdef CFG_INTERFACE
static void cfg_mlme_event(struct i802_bss *bss,
		       enum nl80211_commands cmd, 
		       char * data,
		       int len)
{
	struct wpa_driver_nl80211_data *drv = bss->drv;
	struct nrf_wifi_umac_event_mlme *devent = (struct nrf_wifi_umac_event_mlme *)data;
	const u8 *recv_data = (u8 *)devent->frame.frame;
	int frame_len = devent->frame.frame_len; 

	if (frame_len < 4 + 2 * ETH_ALEN) {
		wpa_printf(MSG_MSGDUMP, "nl80211: MLME event %d (%s) on %s("
			   MACSTR ") - too short",
			   cmd, nl80211_command_to_string(cmd), bss->ifname,
			   MAC2STR(bss->addr));
		return;
	}
	wpa_printf(MSG_MSGDUMP, "nl80211: MLME event %d (%s) on %s(" MACSTR
		   ") A1=" MACSTR " A2=" MACSTR, cmd,
		   nl80211_command_to_string(cmd), bss->ifname,
		   MAC2STR(bss->addr), MAC2STR(recv_data + 4),
		   MAC2STR(recv_data + 4 + ETH_ALEN));

	if (cmd != NRF_WIFI_UMAC_EVENT_FRAME_TX_STATUS  && !(recv_data[4] & 0x01) &&
	    os_memcmp(bss->addr, recv_data + 4, ETH_ALEN) != 0 &&
	    (is_zero_ether_addr(bss->rand_addr) ||
	     os_memcmp(bss->rand_addr, recv_data + 4, ETH_ALEN) != 0) &&
	    os_memcmp(bss->addr, recv_data + 4 + ETH_ALEN, ETH_ALEN) != 0) {
		wpa_printf(MSG_MSGDUMP, "nl80211: %s: Ignore MLME frame event "
			   "for foreign address", bss->ifname);
		return;
	}
	wpa_hexdump(MSG_MSGDUMP, "nl80211: MLME event frame",
		    recv_data, len);
	
	switch (cmd) {
	case NL80211_CMD_AUTHENTICATE:
		mlme_event_auth(drv, (const u8*)recv_data, frame_len);
	break;
	case NL80211_CMD_ASSOCIATE:
		mlme_event_assoc(drv, (const u8*)recv_data, frame_len, NULL, NULL);
	break;
	case NRF_WIFI_UMAC_EVENT_FRAME:
                cfg_mlme_event_mgmt(bss, devent, frame_len);
	break;
        case NRF_WIFI_UMAC_EVENT_FRAME_TX_STATUS:
                cfg_mlme_event_mgmt_tx_status(drv, devent, frame_len);
        break;
	case NL80211_CMD_DEAUTHENTICATE:
		mlme_event_deauth_disassoc(drv, EVENT_DEAUTH,
				(const u8*)recv_data, frame_len);
	break;
	case NL80211_CMD_DISASSOCIATE:
		mlme_event_deauth_disassoc(drv, EVENT_DISASSOC,
				(const u8*)recv_data, frame_len);
	break;
	case NL80211_CMD_UNPROT_DEAUTHENTICATE:
		mlme_event_unprot_disconnect(drv, EVENT_UNPROT_DEAUTH,
				(const u8*)recv_data, frame_len);
	break;
	case NL80211_CMD_UNPROT_DISASSOCIATE:
		mlme_event_unprot_disconnect(drv, EVENT_UNPROT_DISASSOC,
				(const u8*)recv_data, frame_len);
	break;
	default:
		break;
	}

}
#endif

static void mlme_event(struct i802_bss *bss,
		       enum nl80211_commands cmd, struct nlattr *frame,
		       struct nlattr *addr, struct nlattr *timed_out,
		       struct nlattr *freq, struct nlattr *ack,
		       struct nlattr *cookie, struct nlattr *sig,
		       struct nlattr *wmm, struct nlattr *req_ie)
{
	struct wpa_driver_nl80211_data *drv = bss->drv;
	const u8 *data;
	size_t len;

	if (timed_out && addr) {
		mlme_timeout_event(drv, cmd, addr);
		return;
	}

	if (frame == NULL) {
		wpa_printf(MSG_DEBUG,
			   "nl80211: MLME event %d (%s) without frame data",
			   cmd, nl80211_command_to_string(cmd));
		return;
	}

	data = nla_data(frame);
	len = nla_len(frame);
	if (len < 4 + 2 * ETH_ALEN) {
		wpa_printf(MSG_MSGDUMP, "nl80211: MLME event %d (%s) on %s("
			   MACSTR ") - too short",
			   cmd, nl80211_command_to_string(cmd), bss->ifname,
			   MAC2STR(bss->addr));
		return;
	}
	wpa_printf(MSG_MSGDUMP, "nl80211: MLME event %d (%s) on %s(" MACSTR
		   ") A1=" MACSTR " A2=" MACSTR, cmd,
		   nl80211_command_to_string(cmd), bss->ifname,
		   MAC2STR(bss->addr), MAC2STR(data + 4),
		   MAC2STR(data + 4 + ETH_ALEN));
	if (cmd != NL80211_CMD_FRAME_TX_STATUS && !(data[4] & 0x01) &&
	    os_memcmp(bss->addr, data + 4, ETH_ALEN) != 0 &&
	    (is_zero_ether_addr(bss->rand_addr) ||
	     os_memcmp(bss->rand_addr, data + 4, ETH_ALEN) != 0) &&
	    os_memcmp(bss->addr, data + 4 + ETH_ALEN, ETH_ALEN) != 0) {
		wpa_printf(MSG_MSGDUMP, "nl80211: %s: Ignore MLME frame event "
			   "for foreign address", bss->ifname);
		return;
	}
	wpa_hexdump(MSG_MSGDUMP, "nl80211: MLME event frame",
		    nla_data(frame), nla_len(frame));

	switch (cmd) {
	case NL80211_CMD_AUTHENTICATE:
		mlme_event_auth(drv, nla_data(frame), nla_len(frame));
		break;
	case NL80211_CMD_ASSOCIATE:
		mlme_event_assoc(drv, nla_data(frame), nla_len(frame), wmm,
				 req_ie);
		break;
	case NL80211_CMD_DEAUTHENTICATE:
		mlme_event_deauth_disassoc(drv, EVENT_DEAUTH,
					   nla_data(frame), nla_len(frame));
		break;
	case NL80211_CMD_DISASSOCIATE:
		mlme_event_deauth_disassoc(drv, EVENT_DISASSOC,
					   nla_data(frame), nla_len(frame));
		break;
	case NL80211_CMD_FRAME:
		mlme_event_mgmt(bss, freq, sig, nla_data(frame),
				nla_len(frame));
		break;
	case NL80211_CMD_FRAME_TX_STATUS:
		mlme_event_mgmt_tx_status(drv, cookie, nla_data(frame),
					  nla_len(frame), ack);
		break;
	case NL80211_CMD_UNPROT_DEAUTHENTICATE:
		mlme_event_unprot_disconnect(drv, EVENT_UNPROT_DEAUTH,
					     nla_data(frame), nla_len(frame));
		break;
	case NL80211_CMD_UNPROT_DISASSOCIATE:
		mlme_event_unprot_disconnect(drv, EVENT_UNPROT_DISASSOC,
					     nla_data(frame), nla_len(frame));
		break;
	case NL80211_CMD_UNPROT_BEACON:
		mlme_event_unprot_beacon(drv, nla_data(frame), nla_len(frame));
		break;
	default:
		break;
	}
}


static void mlme_event_michael_mic_failure(struct i802_bss *bss,
					   struct nlattr *tb[])
{
	union wpa_event_data data;

	wpa_printf(MSG_DEBUG, "nl80211: MLME event Michael MIC failure");
	os_memset(&data, 0, sizeof(data));
	if (tb[NL80211_ATTR_MAC]) {
		wpa_hexdump(MSG_DEBUG, "nl80211: Source MAC address",
			    nla_data(tb[NL80211_ATTR_MAC]),
			    nla_len(tb[NL80211_ATTR_MAC]));
		data.michael_mic_failure.src = nla_data(tb[NL80211_ATTR_MAC]);
	}
	if (tb[NL80211_ATTR_KEY_SEQ]) {
		wpa_hexdump(MSG_DEBUG, "nl80211: TSC",
			    nla_data(tb[NL80211_ATTR_KEY_SEQ]),
			    nla_len(tb[NL80211_ATTR_KEY_SEQ]));
	}
	if (tb[NL80211_ATTR_KEY_TYPE]) {
		enum nl80211_key_type key_type =
			nla_get_u32(tb[NL80211_ATTR_KEY_TYPE]);
		wpa_printf(MSG_DEBUG, "nl80211: Key Type %d", key_type);
		if (key_type == NL80211_KEYTYPE_PAIRWISE)
			data.michael_mic_failure.unicast = 1;
	} else
		data.michael_mic_failure.unicast = 1;

	if (tb[NL80211_ATTR_KEY_IDX]) {
		u8 key_id = nla_get_u8(tb[NL80211_ATTR_KEY_IDX]);
		wpa_printf(MSG_DEBUG, "nl80211: Key Id %d", key_id);
	}

	wpa_supplicant_event(bss->ctx, EVENT_MICHAEL_MIC_FAILURE, &data);
}


static void mlme_event_join_ibss(struct wpa_driver_nl80211_data *drv,
				 struct nlattr *tb[])
{
	unsigned int freq;
	union wpa_event_data event;

	if (tb[NL80211_ATTR_MAC] == NULL) {
		wpa_printf(MSG_DEBUG, "nl80211: No address in IBSS joined "
			   "event");
		return;
	}
	os_memcpy(drv->bssid, nla_data(tb[NL80211_ATTR_MAC]), ETH_ALEN);

	drv->associated = 1;
	wpa_printf(MSG_DEBUG, "nl80211: IBSS " MACSTR " joined",
		   MAC2STR(drv->bssid));

	freq = nl80211_get_assoc_freq(drv);
	if (freq) {
		wpa_printf(MSG_DEBUG, "nl80211: IBSS on frequency %u MHz",
			   freq);
		drv->first_bss->freq = freq;
	}

	os_memset(&event, 0, sizeof(event));
	event.assoc_info.freq = freq;

	wpa_supplicant_event(drv->ctx, EVENT_ASSOC, &event);
}

#ifdef CFG_INTERFACE
static void cfg_mlme_event_remain_on_channel(struct wpa_driver_nl80211_data *drv,
					 int cancel_event, void *msg)
{
	unsigned int freq, chan_type, duration;
	union wpa_event_data data;
	u64 cookie;
	unsigned long long host_cookie = 0;
	struct cookie_info *cookie_info = NULL;
        struct cookie_info *tmp = NULL;
        int match_cookie_found = 0;
	struct nrf_wifi_event_remain_on_channel *roc_event = (struct nrf_wifi_event_remain_on_channel*)msg;

	 if (dl_list_empty(&drv->global->cookie_list))
                return;
	
	dl_list_for_each_safe(cookie_info, tmp, &drv->global->cookie_list,
			      struct cookie_info, list) {
        	if (cookie_info) {
        		if (cookie_info->rpu_cookie != roc_event->cookie)
                        	continue;
			host_cookie = cookie_info->host_cookie;
                        match_cookie_found = 1;
			if (cancel_event) {
				dl_list_del(&cookie_info->list);
				os_free(cookie_info);
			}
                        break;
                }
	}

	if (match_cookie_found) {
		if (roc_event->valid_fields & NRF_WIFI_EVENT_ROC_FREQ_VALID)
			freq = roc_event->frequency;
		else
			freq = 0;

		if (roc_event->valid_fields & NRF_WIFI_EVENT_ROC_CH_TYPE_VALID) 
			chan_type = roc_event->ch_type;
		else
			chan_type = 0;

		if (roc_event->valid_fields & NRF_WIFI_EVENT_ROC_DURATION_VALID)
			duration =  roc_event->dur;
		else
			duration = 0;

		cookie = host_cookie;
	}
	wpa_printf(MSG_DEBUG, "nl80211: Remain-on-channel event (cancel=%d "
		   "freq=%u channel_type=%u duration=%u cookie=0x%llx (%s))",
		   cancel_event, freq, chan_type, duration,
		   (long long unsigned int) cookie,
		   cookie == drv->remain_on_chan_cookie ? "match" : "unknown");
	
	if (cookie != drv->remain_on_chan_cookie)
		return; /* not for us */
	
	if (cancel_event)
		drv->pending_remain_on_chan = 0;

	os_memset(&data, 0, sizeof(data));
	data.remain_on_channel.freq = freq;
	data.remain_on_channel.duration = duration;
	wpa_supplicant_event(drv->ctx, cancel_event ?
			     EVENT_CANCEL_REMAIN_ON_CHANNEL :
			     EVENT_REMAIN_ON_CHANNEL, &data);
}
#endif

static void mlme_event_remain_on_channel(struct wpa_driver_nl80211_data *drv,
					 int cancel_event, struct nlattr *tb[])
{
	unsigned int freq, chan_type, duration;
	union wpa_event_data data;
	u64 cookie;

	if (tb[NL80211_ATTR_WIPHY_FREQ])
		freq = nla_get_u32(tb[NL80211_ATTR_WIPHY_FREQ]);
	else
		freq = 0;

	if (tb[NL80211_ATTR_WIPHY_CHANNEL_TYPE])
		chan_type = nla_get_u32(tb[NL80211_ATTR_WIPHY_CHANNEL_TYPE]);
	else
		chan_type = 0;

	if (tb[NL80211_ATTR_DURATION])
		duration = nla_get_u32(tb[NL80211_ATTR_DURATION]);
	else
		duration = 0;

	if (tb[NL80211_ATTR_COOKIE])
		cookie = nla_get_u64(tb[NL80211_ATTR_COOKIE]);
	else
		cookie = 0;

	wpa_printf(MSG_DEBUG, "nl80211: Remain-on-channel event (cancel=%d "
		   "freq=%u channel_type=%u duration=%u cookie=0x%llx (%s))",
		   cancel_event, freq, chan_type, duration,
		   (long long unsigned int) cookie,
		   cookie == drv->remain_on_chan_cookie ? "match" : "unknown");

	if (cookie != drv->remain_on_chan_cookie)
		return; /* not for us */

	if (cancel_event)
		drv->pending_remain_on_chan = 0;

	os_memset(&data, 0, sizeof(data));
	data.remain_on_channel.freq = freq;
	data.remain_on_channel.duration = duration;
	wpa_supplicant_event(drv->ctx, cancel_event ?
			     EVENT_CANCEL_REMAIN_ON_CHANNEL :
			     EVENT_REMAIN_ON_CHANNEL, &data);
}


static void mlme_event_ft_event(struct wpa_driver_nl80211_data *drv,
				struct nlattr *tb[])
{
	union wpa_event_data data;

	os_memset(&data, 0, sizeof(data));

	if (tb[NL80211_ATTR_IE]) {
		data.ft_ies.ies = nla_data(tb[NL80211_ATTR_IE]);
		data.ft_ies.ies_len = nla_len(tb[NL80211_ATTR_IE]);
	}

	if (tb[NL80211_ATTR_IE_RIC]) {
		data.ft_ies.ric_ies = nla_data(tb[NL80211_ATTR_IE_RIC]);
		data.ft_ies.ric_ies_len = nla_len(tb[NL80211_ATTR_IE_RIC]);
	}

	if (tb[NL80211_ATTR_MAC])
		os_memcpy(data.ft_ies.target_ap,
			  nla_data(tb[NL80211_ATTR_MAC]), ETH_ALEN);

	wpa_printf(MSG_DEBUG, "nl80211: FT event target_ap " MACSTR,
		   MAC2STR(data.ft_ies.target_ap));

	wpa_supplicant_event(drv->ctx, EVENT_FT_RESPONSE, &data);
}


static void mlme_event_dh_event(struct wpa_driver_nl80211_data *drv,
				struct i802_bss *bss,
				struct nlattr *tb[])
{
	union wpa_event_data data;

	if (!is_ap_interface(drv->nlmode))
		return;
	if (!tb[NL80211_ATTR_MAC] || !tb[NL80211_ATTR_IE])
		return;

	os_memset(&data, 0, sizeof(data));
	data.update_dh.peer = nla_data(tb[NL80211_ATTR_MAC]);
	data.update_dh.ie = nla_data(tb[NL80211_ATTR_IE]);
	data.update_dh.ie_len = nla_len(tb[NL80211_ATTR_IE]);

	wpa_printf(MSG_DEBUG, "nl80211: DH event - peer " MACSTR,
		   MAC2STR(data.update_dh.peer));

	wpa_supplicant_event(bss->ctx, EVENT_UPDATE_DH, &data);
}


#ifdef CFG_INTERFACE
static void send_scan_event_cfg(struct wpa_driver_nl80211_data *drv, int aborted,
			    void *msg_buf,
			    int external_scan)
{
	union wpa_event_data event;
	struct scan_info *info;
#define MAX_REPORT_FREQS 50
	int freqs[MAX_REPORT_FREQS];
	int num_freqs = 0;
#if 0
	struct nrf_wifi_umac_event_trigger_scan *scan_complete_event = 
		(struct nrf_wifi_umac_event_trigger_scan *) msg_buf;
	int i = 0;
#endif
	if (!external_scan && drv->scan_for_auth) {
		drv->scan_for_auth = 0;
		wpa_printf(MSG_DEBUG, "nl80211: Scan results for missing "
			   "cfg80211 BSS entry");
		wpa_driver_nl80211_authenticate_retry(drv);
		return;
	}

	os_memset(&event, 0, sizeof(event));
	info = &event.scan_info;
	info->aborted = aborted;
	info->external_scan = external_scan;
	info->nl_scan_event = 1;

	wpa_printf(MSG_DEBUG, "Scan done received............Now get scan results");
#if 0
	for (i = 0; i < scan_complete_event->num_scan_ssid; i++) {
		struct wpa_driver_scan_ssid *s =
			&info->ssids[info->num_ssids];
		memcpy(s->ssid, scan_complete_event->scan_ssid[i].nrf_wifi_ssid,
			scan_complete_event->scan_ssid[i].nrf_wifi_ssid_len);
		s->ssid_len = scan_complete_event->scan_ssid[i].nrf_wifi_ssid_len;

		wpa_printf(MSG_DEBUG, "nl80211: Scan probed for SSID '%s'",
			   wpa_ssid_txt(s->ssid, s->ssid_len));
			info->num_ssids++;
			if (info->num_ssids == WPAS_MAX_SCAN_SSIDS)
				break;
	}
	
	for (i = 0; i < scan_complete_event->num_scan_frequencies; i++) {
		freqs[num_freqs] =
			scan_complete_event->scan_frequencies[i];
		num_freqs++;
		if (num_freqs == MAX_REPORT_FREQS - 1)
			break;
	}
	info->freqs = freqs;
	info->num_freqs = scan_complete_event->num_scan_frequencies;

	printf("TODO: why scan frequencies are not getting updated\n");
	
	wpa_printf(MSG_DEBUG, "nl80211: Scan included frequencies:%s",
		   (char*) scan_complete_event->scan_frequencies);

#endif
#if 0
	if (tb[NL80211_ATTR_SCAN_START_TIME_TSF] &&
	    tb[NL80211_ATTR_SCAN_START_TIME_TSF_BSSID]) {
		info->scan_start_tsf =
			nla_get_u64(tb[NL80211_ATTR_SCAN_START_TIME_TSF]);
		os_memcpy(info->scan_start_tsf_bssid,
			  nla_data(tb[NL80211_ATTR_SCAN_START_TIME_TSF_BSSID]),
			  ETH_ALEN);
	}
#endif
	
#ifdef BSS_OPTIMIZATION
	if ((((struct nrf_wifi_umac_event_scan_done *)msg_buf)->scan_type == 0)) {
		wpa_printf(MSG_DEBUG, "DISPLAY Scan done received");
		    	fprintf(stdout, "\nDISPLAY-SCAN-DONE\n");
			drv->scan_state = SCAN_COMPLETED;
			wpa_supplicant_event_display_scan_done(drv->ctx,  EVENT_SCAN_RESULTS, &event);
			wpa_supplicant_retrieve_display_scan_results(drv->ctx);
		    	fprintf(stdout, "\nAUTO-RETRIEVE-DISPLAY-SCAN-RESULT\n");
	    return;

	}
#endif
	fprintf(stdout, "\nCONNECT-SCAN-DONE\n");
	wpa_supplicant_event(drv->ctx, EVENT_SCAN_RESULTS, &event);

}
#endif
static void send_scan_event(struct wpa_driver_nl80211_data *drv, int aborted,
			    struct nlattr *tb[], int external_scan)
{
	union wpa_event_data event;
	struct nlattr *nl;
	int rem;
	struct scan_info *info;
#define MAX_REPORT_FREQS 100
	int freqs[MAX_REPORT_FREQS];
	int num_freqs = 0;

	if (!external_scan && drv->scan_for_auth) {
		drv->scan_for_auth = 0;
		wpa_printf(MSG_DEBUG, "nl80211: Scan results for missing "
			   "cfg80211 BSS entry");
		wpa_driver_nl80211_authenticate_retry(drv);
		return;
	}

	os_memset(&event, 0, sizeof(event));
	info = &event.scan_info;
	info->aborted = aborted;
	info->external_scan = external_scan;
	info->nl_scan_event = 1;

	if (tb[NL80211_ATTR_SCAN_SSIDS]) {
		nla_for_each_nested(nl, tb[NL80211_ATTR_SCAN_SSIDS], rem) {
			struct wpa_driver_scan_ssid *s =
				&info->ssids[info->num_ssids];
			s->ssid = nla_data(nl);
			s->ssid_len = nla_len(nl);
			wpa_printf(MSG_DEBUG, "nl80211: Scan probed for SSID '%s'",
				   wpa_ssid_txt(s->ssid, s->ssid_len));
			info->num_ssids++;
			if (info->num_ssids == WPAS_MAX_SCAN_SSIDS)
				break;
		}
	}
	if (tb[NL80211_ATTR_SCAN_FREQUENCIES]) {
		char msg[500], *pos, *end;
		int res;

		pos = msg;
		end = pos + sizeof(msg);
		*pos = '\0';

		nla_for_each_nested(nl, tb[NL80211_ATTR_SCAN_FREQUENCIES], rem)
		{
			freqs[num_freqs] = nla_get_u32(nl);
			res = os_snprintf(pos, end - pos, " %d",
					  freqs[num_freqs]);
			if (!os_snprintf_error(end - pos, res))
				pos += res;
			num_freqs++;
			if (num_freqs == MAX_REPORT_FREQS - 1)
				break;
		}
		info->freqs = freqs;
		info->num_freqs = num_freqs;
		wpa_printf(MSG_DEBUG, "nl80211: Scan included frequencies:%s",
			   msg);
	}

	if (tb[NL80211_ATTR_SCAN_START_TIME_TSF] &&
	    tb[NL80211_ATTR_SCAN_START_TIME_TSF_BSSID]) {
		info->scan_start_tsf =
			nla_get_u64(tb[NL80211_ATTR_SCAN_START_TIME_TSF]);
		os_memcpy(info->scan_start_tsf_bssid,
			  nla_data(tb[NL80211_ATTR_SCAN_START_TIME_TSF_BSSID]),
			  ETH_ALEN);
	}

	wpa_supplicant_event(drv->ctx, EVENT_SCAN_RESULTS, &event);
}


static void nl80211_cqm_event(struct wpa_driver_nl80211_data *drv,
			      struct nlattr *tb[])
{
	static struct nla_policy cqm_policy[NL80211_ATTR_CQM_MAX + 1] = {
		[NL80211_ATTR_CQM_RSSI_THOLD] = { .type = NLA_U32 },
		[NL80211_ATTR_CQM_RSSI_HYST] = { .type = NLA_U8 },
		[NL80211_ATTR_CQM_RSSI_THRESHOLD_EVENT] = { .type = NLA_U32 },
		[NL80211_ATTR_CQM_PKT_LOSS_EVENT] = { .type = NLA_U32 },
		[NL80211_ATTR_CQM_TXE_RATE] = { .type = NLA_U32 },
		[NL80211_ATTR_CQM_TXE_PKTS] = { .type = NLA_U32 },
		[NL80211_ATTR_CQM_TXE_INTVL] = { .type = NLA_U32 },
		[NL80211_ATTR_CQM_BEACON_LOSS_EVENT] = { .type = NLA_FLAG },
	};
	struct nlattr *cqm[NL80211_ATTR_CQM_MAX + 1];
	enum nl80211_cqm_rssi_threshold_event event;
	union wpa_event_data ed;
	int res;

	if (tb[NL80211_ATTR_CQM] == NULL ||
	    nla_parse_nested(cqm, NL80211_ATTR_CQM_MAX, tb[NL80211_ATTR_CQM],
			     cqm_policy)) {
		wpa_printf(MSG_DEBUG, "nl80211: Ignore invalid CQM event");
		return;
	}

	os_memset(&ed, 0, sizeof(ed));

	if (cqm[NL80211_ATTR_CQM_PKT_LOSS_EVENT]) {
		if (!tb[NL80211_ATTR_MAC])
			return;
		os_memcpy(ed.low_ack.addr, nla_data(tb[NL80211_ATTR_MAC]),
			  ETH_ALEN);
		ed.low_ack.num_packets =
			nla_get_u32(cqm[NL80211_ATTR_CQM_PKT_LOSS_EVENT]);
		wpa_printf(MSG_DEBUG, "nl80211: Packet loss event for " MACSTR
			   " (num_packets %u)",
			   MAC2STR(ed.low_ack.addr), ed.low_ack.num_packets);
		wpa_supplicant_event(drv->ctx, EVENT_STATION_LOW_ACK, &ed);
		return;
	}

	if (cqm[NL80211_ATTR_CQM_BEACON_LOSS_EVENT]) {
		wpa_printf(MSG_DEBUG, "nl80211: Beacon loss event");
		wpa_supplicant_event(drv->ctx, EVENT_BEACON_LOSS, NULL);
		return;
	}

	if (cqm[NL80211_ATTR_CQM_TXE_RATE] &&
	    cqm[NL80211_ATTR_CQM_TXE_PKTS] &&
	    cqm[NL80211_ATTR_CQM_TXE_INTVL] &&
	    cqm[NL80211_ATTR_MAC]) {
		wpa_printf(MSG_DEBUG, "nl80211: CQM TXE event for " MACSTR
			   " (rate: %u pkts: %u interval: %u)",
			   MAC2STR((u8 *) nla_data(cqm[NL80211_ATTR_MAC])),
			   nla_get_u32(cqm[NL80211_ATTR_CQM_TXE_RATE]),
			   nla_get_u32(cqm[NL80211_ATTR_CQM_TXE_PKTS]),
			   nla_get_u32(cqm[NL80211_ATTR_CQM_TXE_INTVL]));
		return;
	}

	if (cqm[NL80211_ATTR_CQM_RSSI_THRESHOLD_EVENT] == NULL) {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Not a CQM RSSI threshold event");
		return;
	}
	event = nla_get_u32(cqm[NL80211_ATTR_CQM_RSSI_THRESHOLD_EVENT]);

	if (event == NL80211_CQM_RSSI_THRESHOLD_EVENT_HIGH) {
		wpa_printf(MSG_DEBUG, "nl80211: Connection quality monitor "
			   "event: RSSI high");
		ed.signal_change.above_threshold = 1;
	} else if (event == NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW) {
		wpa_printf(MSG_DEBUG, "nl80211: Connection quality monitor "
			   "event: RSSI low");
		ed.signal_change.above_threshold = 0;
	} else {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Unknown CQM RSSI threshold event: %d",
			   event);
		return;
	}

	/*
	 * nl80211_get_link_signal() and nl80211_get_link_noise() set default
	 * values in case querying the driver fails.
	 */
	res = nl80211_get_link_signal(drv, &ed.signal_change);
	if (res == 0) {
		wpa_printf(MSG_DEBUG, "nl80211: Signal: %d dBm  txrate: %d",
			   ed.signal_change.current_signal,
			   ed.signal_change.current_txrate);
	} else {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Querying the driver for signal info failed");
	}

	res = nl80211_get_link_noise(drv, &ed.signal_change);
	if (res == 0) {
		wpa_printf(MSG_DEBUG, "nl80211: Noise: %d dBm",
			   ed.signal_change.current_noise);
	} else {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Querying the driver for noise info failed");
	}

	wpa_supplicant_event(drv->ctx, EVENT_SIGNAL_CHANGE, &ed);
}


static void nl80211_new_peer_candidate(struct wpa_driver_nl80211_data *drv,
				       struct nlattr **tb)
{
	const u8 *addr;
	union wpa_event_data data;

	if (drv->nlmode != NL80211_IFTYPE_MESH_POINT ||
	    !tb[NL80211_ATTR_MAC] || !tb[NL80211_ATTR_IE])
		return;

	addr = nla_data(tb[NL80211_ATTR_MAC]);
	wpa_printf(MSG_DEBUG, "nl80211: New peer candidate " MACSTR,
		   MAC2STR(addr));

	os_memset(&data, 0, sizeof(data));
	data.mesh_peer.peer = addr;
	data.mesh_peer.ies = nla_data(tb[NL80211_ATTR_IE]);
	data.mesh_peer.ie_len = nla_len(tb[NL80211_ATTR_IE]);
	wpa_supplicant_event(drv->ctx, EVENT_NEW_PEER_CANDIDATE, &data);
}


static void nl80211_new_station_event(struct wpa_driver_nl80211_data *drv,
				      struct i802_bss *bss,
				      struct nlattr **tb)
{
	u8 *addr;
	union wpa_event_data data;

	if (tb[NL80211_ATTR_MAC] == NULL)
		return;
	addr = nla_data(tb[NL80211_ATTR_MAC]);
	wpa_printf(MSG_DEBUG, "nl80211: New station " MACSTR, MAC2STR(addr));

	if (is_ap_interface(drv->nlmode) && drv->device_ap_sme) {
		u8 *ies = NULL;
		size_t ies_len = 0;
		if (tb[NL80211_ATTR_IE]) {
			ies = nla_data(tb[NL80211_ATTR_IE]);
			ies_len = nla_len(tb[NL80211_ATTR_IE]);
		}
		wpa_hexdump(MSG_DEBUG, "nl80211: Assoc Req IEs", ies, ies_len);
		drv_event_assoc(bss->ctx, addr, ies, ies_len, 0);
		return;
	}

	if (drv->nlmode != NL80211_IFTYPE_ADHOC)
		return;

	os_memset(&data, 0, sizeof(data));
	os_memcpy(data.ibss_rsn_start.peer, addr, ETH_ALEN);
	wpa_supplicant_event(bss->ctx, EVENT_IBSS_RSN_START, &data);
}


static void nl80211_del_station_event(struct wpa_driver_nl80211_data *drv,
				      struct i802_bss *bss,
				      struct nlattr **tb)
{
	u8 *addr;
	union wpa_event_data data;

	if (tb[NL80211_ATTR_MAC] == NULL)
		return;
	addr = nla_data(tb[NL80211_ATTR_MAC]);
	wpa_printf(MSG_DEBUG, "nl80211: Delete station " MACSTR,
		   MAC2STR(addr));

	if (is_ap_interface(drv->nlmode) && drv->device_ap_sme) {
		drv_event_disassoc(bss->ctx, addr);
		return;
	}

	if (drv->nlmode != NL80211_IFTYPE_ADHOC)
		return;

	os_memset(&data, 0, sizeof(data));
	os_memcpy(data.ibss_peer_lost.peer, addr, ETH_ALEN);
	wpa_supplicant_event(bss->ctx, EVENT_IBSS_PEER_LOST, &data);
}


static void nl80211_rekey_offload_event(struct wpa_driver_nl80211_data *drv,
					struct nlattr **tb)
{
	struct nlattr *rekey_info[NUM_NL80211_REKEY_DATA];
	static struct nla_policy rekey_policy[NUM_NL80211_REKEY_DATA] = {
		[NL80211_REKEY_DATA_KEK] = {
			.minlen = NL80211_KEK_LEN,
			.maxlen = NL80211_KEK_LEN,
		},
		[NL80211_REKEY_DATA_KCK] = {
			.minlen = NL80211_KCK_LEN,
			.maxlen = NL80211_KCK_LEN,
		},
		[NL80211_REKEY_DATA_REPLAY_CTR] = {
			.minlen = NL80211_REPLAY_CTR_LEN,
			.maxlen = NL80211_REPLAY_CTR_LEN,
		},
	};
	union wpa_event_data data;

	if (!tb[NL80211_ATTR_MAC] ||
	    !tb[NL80211_ATTR_REKEY_DATA] ||
	    nla_parse_nested(rekey_info, MAX_NL80211_REKEY_DATA,
			     tb[NL80211_ATTR_REKEY_DATA], rekey_policy) ||
	    !rekey_info[NL80211_REKEY_DATA_REPLAY_CTR])
		return;

	os_memset(&data, 0, sizeof(data));
	data.driver_gtk_rekey.bssid = nla_data(tb[NL80211_ATTR_MAC]);
	wpa_printf(MSG_DEBUG, "nl80211: Rekey offload event for BSSID " MACSTR,
		   MAC2STR(data.driver_gtk_rekey.bssid));
	data.driver_gtk_rekey.replay_ctr =
		nla_data(rekey_info[NL80211_REKEY_DATA_REPLAY_CTR]);
	wpa_hexdump(MSG_DEBUG, "nl80211: Rekey offload - Replay Counter",
		    data.driver_gtk_rekey.replay_ctr, NL80211_REPLAY_CTR_LEN);
	wpa_supplicant_event(drv->ctx, EVENT_DRIVER_GTK_REKEY, &data);
}


static void nl80211_pmksa_candidate_event(struct wpa_driver_nl80211_data *drv,
					  struct nlattr **tb)
{
	struct nlattr *cand[NUM_NL80211_PMKSA_CANDIDATE];
	static struct nla_policy cand_policy[NUM_NL80211_PMKSA_CANDIDATE] = {
		[NL80211_PMKSA_CANDIDATE_INDEX] = { .type = NLA_U32 },
		[NL80211_PMKSA_CANDIDATE_BSSID] = {
			.minlen = ETH_ALEN,
			.maxlen = ETH_ALEN,
		},
		[NL80211_PMKSA_CANDIDATE_PREAUTH] = { .type = NLA_FLAG },
	};
	union wpa_event_data data;

	wpa_printf(MSG_DEBUG, "nl80211: PMKSA candidate event");

	if (!tb[NL80211_ATTR_PMKSA_CANDIDATE] ||
	    nla_parse_nested(cand, MAX_NL80211_PMKSA_CANDIDATE,
			     tb[NL80211_ATTR_PMKSA_CANDIDATE], cand_policy) ||
	    !cand[NL80211_PMKSA_CANDIDATE_INDEX] ||
	    !cand[NL80211_PMKSA_CANDIDATE_BSSID])
		return;

	os_memset(&data, 0, sizeof(data));
	os_memcpy(data.pmkid_candidate.bssid,
		  nla_data(cand[NL80211_PMKSA_CANDIDATE_BSSID]), ETH_ALEN);
	data.pmkid_candidate.index =
		nla_get_u32(cand[NL80211_PMKSA_CANDIDATE_INDEX]);
	data.pmkid_candidate.preauth =
		cand[NL80211_PMKSA_CANDIDATE_PREAUTH] != NULL;
	wpa_supplicant_event(drv->ctx, EVENT_PMKID_CANDIDATE, &data);
}


static void nl80211_client_probe_event(struct wpa_driver_nl80211_data *drv,
				       struct nlattr **tb)
{
	union wpa_event_data data;
	const u8 *addr;
	u64 cookie = 0;

	addr = nla_data(tb[NL80211_ATTR_MAC]);
	if (!addr)
		return;
	if (tb[NL80211_ATTR_COOKIE])
		cookie = nla_get_u64(tb[NL80211_ATTR_COOKIE]);
	wpa_printf(MSG_DEBUG, "nl80211: Probe client event (addr=" MACSTR
		   " ack=%d cookie=%llu)", MAC2STR(addr),
		   tb[NL80211_ATTR_ACK] != NULL,
		   (long long unsigned int) cookie);
	if (!tb[NL80211_ATTR_ACK])
		return;

	os_memset(&data, 0, sizeof(data));
	os_memcpy(data.client_poll.addr, addr, ETH_ALEN);
	wpa_supplicant_event(drv->ctx, EVENT_DRIVER_CLIENT_POLL_OK, &data);
}


static void nl80211_tdls_oper_event(struct wpa_driver_nl80211_data *drv,
				    struct nlattr **tb)
{
	union wpa_event_data data;

	wpa_printf(MSG_DEBUG, "nl80211: TDLS operation event");

	if (!tb[NL80211_ATTR_MAC] || !tb[NL80211_ATTR_TDLS_OPERATION])
		return;

	os_memset(&data, 0, sizeof(data));
	os_memcpy(data.tdls.peer, nla_data(tb[NL80211_ATTR_MAC]), ETH_ALEN);
	switch (nla_get_u8(tb[NL80211_ATTR_TDLS_OPERATION])) {
	case NL80211_TDLS_SETUP:
		wpa_printf(MSG_DEBUG, "nl80211: TDLS setup request for peer "
			   MACSTR, MAC2STR(data.tdls.peer));
		data.tdls.oper = TDLS_REQUEST_SETUP;
		break;
	case NL80211_TDLS_TEARDOWN:
		wpa_printf(MSG_DEBUG, "nl80211: TDLS teardown request for peer "
			   MACSTR, MAC2STR(data.tdls.peer));
		data.tdls.oper = TDLS_REQUEST_TEARDOWN;
		break;
	case NL80211_TDLS_DISCOVERY_REQ:
		wpa_printf(MSG_DEBUG,
			   "nl80211: TDLS discovery request for peer " MACSTR,
			   MAC2STR(data.tdls.peer));
		data.tdls.oper = TDLS_REQUEST_DISCOVER;
		break;
	default:
		wpa_printf(MSG_DEBUG, "nl80211: Unsupported TDLS operatione "
			   "event");
		return;
	}
	if (tb[NL80211_ATTR_REASON_CODE]) {
		data.tdls.reason_code =
			nla_get_u16(tb[NL80211_ATTR_REASON_CODE]);
	}

	wpa_supplicant_event(drv->ctx, EVENT_TDLS, &data);
}


static void nl80211_stop_ap(struct wpa_driver_nl80211_data *drv,
			    struct nlattr **tb)
{
	wpa_supplicant_event(drv->ctx, EVENT_INTERFACE_UNAVAILABLE, NULL);
}


static void nl80211_connect_failed_event(struct wpa_driver_nl80211_data *drv,
					 struct nlattr **tb)
{
	union wpa_event_data data;
	u32 reason;

	wpa_printf(MSG_DEBUG, "nl80211: Connect failed event");

	if (!tb[NL80211_ATTR_MAC] || !tb[NL80211_ATTR_CONN_FAILED_REASON])
		return;

	os_memset(&data, 0, sizeof(data));
	os_memcpy(data.connect_failed_reason.addr,
		  nla_data(tb[NL80211_ATTR_MAC]), ETH_ALEN);

	reason = nla_get_u32(tb[NL80211_ATTR_CONN_FAILED_REASON]);
	switch (reason) {
	case NL80211_CONN_FAIL_MAX_CLIENTS:
		wpa_printf(MSG_DEBUG, "nl80211: Max client reached");
		data.connect_failed_reason.code = MAX_CLIENT_REACHED;
		break;
	case NL80211_CONN_FAIL_BLOCKED_CLIENT:
		wpa_printf(MSG_DEBUG, "nl80211: Blocked client " MACSTR
			   " tried to connect",
			   MAC2STR(data.connect_failed_reason.addr));
		data.connect_failed_reason.code = BLOCKED_CLIENT;
		break;
	default:
		wpa_printf(MSG_DEBUG, "nl8021l: Unknown connect failed reason "
			   "%u", reason);
		return;
	}

	wpa_supplicant_event(drv->ctx, EVENT_CONNECT_FAILED_REASON, &data);
}


static void nl80211_radar_event(struct wpa_driver_nl80211_data *drv,
				struct nlattr **tb)
{
	union wpa_event_data data;
	enum nl80211_radar_event event_type;

	if (!tb[NL80211_ATTR_WIPHY_FREQ] || !tb[NL80211_ATTR_RADAR_EVENT])
		return;

	os_memset(&data, 0, sizeof(data));
	data.dfs_event.freq = nla_get_u32(tb[NL80211_ATTR_WIPHY_FREQ]);
	event_type = nla_get_u32(tb[NL80211_ATTR_RADAR_EVENT]);

	/* Check HT params */
	if (tb[NL80211_ATTR_WIPHY_CHANNEL_TYPE]) {
		data.dfs_event.ht_enabled = 1;
		data.dfs_event.chan_offset = 0;

		switch (nla_get_u32(tb[NL80211_ATTR_WIPHY_CHANNEL_TYPE])) {
		case NL80211_CHAN_NO_HT:
			data.dfs_event.ht_enabled = 0;
			break;
		case NL80211_CHAN_HT20:
			break;
		case NL80211_CHAN_HT40PLUS:
			data.dfs_event.chan_offset = 1;
			break;
		case NL80211_CHAN_HT40MINUS:
			data.dfs_event.chan_offset = -1;
			break;
		}
	}

	/* Get VHT params */
	if (tb[NL80211_ATTR_CHANNEL_WIDTH])
		data.dfs_event.chan_width =
			convert2width(nla_get_u32(
					      tb[NL80211_ATTR_CHANNEL_WIDTH]));
	if (tb[NL80211_ATTR_CENTER_FREQ1])
		data.dfs_event.cf1 = nla_get_u32(tb[NL80211_ATTR_CENTER_FREQ1]);
	if (tb[NL80211_ATTR_CENTER_FREQ2])
		data.dfs_event.cf2 = nla_get_u32(tb[NL80211_ATTR_CENTER_FREQ2]);

	wpa_printf(MSG_DEBUG, "nl80211: DFS event on freq %d MHz, ht: %d, offset: %d, width: %d, cf1: %dMHz, cf2: %dMHz",
		   data.dfs_event.freq, data.dfs_event.ht_enabled,
		   data.dfs_event.chan_offset, data.dfs_event.chan_width,
		   data.dfs_event.cf1, data.dfs_event.cf2);

	switch (event_type) {
	case NL80211_RADAR_DETECTED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_RADAR_DETECTED, &data);
		break;
	case NL80211_RADAR_CAC_FINISHED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_CAC_FINISHED, &data);
		break;
	case NL80211_RADAR_CAC_ABORTED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_CAC_ABORTED, &data);
		break;
	case NL80211_RADAR_NOP_FINISHED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_NOP_FINISHED, &data);
		break;
	case NL80211_RADAR_PRE_CAC_EXPIRED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_PRE_CAC_EXPIRED,
				     &data);
		break;
	case NL80211_RADAR_CAC_STARTED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_CAC_STARTED, &data);
		break;
	default:
		wpa_printf(MSG_DEBUG, "nl80211: Unknown radar event %d "
			   "received", event_type);
		break;
	}
}


static void nl80211_spurious_frame(struct i802_bss *bss, struct nlattr **tb,
				   int wds)
{
	struct wpa_driver_nl80211_data *drv = bss->drv;
	union wpa_event_data event;

	if (!tb[NL80211_ATTR_MAC])
		return;

	os_memset(&event, 0, sizeof(event));
	event.rx_from_unknown.bssid = bss->addr;
	event.rx_from_unknown.addr = nla_data(tb[NL80211_ATTR_MAC]);
	event.rx_from_unknown.wds = wds;

	wpa_supplicant_event(drv->ctx, EVENT_RX_FROM_UNKNOWN, &event);
}


#ifdef CONFIG_DRIVER_NL80211_QCA

static void qca_nl80211_avoid_freq(struct wpa_driver_nl80211_data *drv,
				   const u8 *data, size_t len)
{
	u32 i, count;
	union wpa_event_data event;
	struct wpa_freq_range *range = NULL;
	const struct qca_avoid_freq_list *freq_range;

	freq_range = (const struct qca_avoid_freq_list *) data;
	if (len < sizeof(freq_range->count))
		return;

	count = freq_range->count;
	if (len < sizeof(freq_range->count) +
	    count * sizeof(struct qca_avoid_freq_range)) {
		wpa_printf(MSG_DEBUG, "nl80211: Ignored too short avoid frequency list (len=%u)",
			   (unsigned int) len);
		return;
	}

	if (count > 0) {
		range = os_calloc(count, sizeof(struct wpa_freq_range));
		if (range == NULL)
			return;
	}

	os_memset(&event, 0, sizeof(event));
	for (i = 0; i < count; i++) {
		unsigned int idx = event.freq_range.num;
		range[idx].min = freq_range->range[i].start_freq;
		range[idx].max = freq_range->range[i].end_freq;
		wpa_printf(MSG_DEBUG, "nl80211: Avoid frequency range: %u-%u",
			   range[idx].min, range[idx].max);
		if (range[idx].min > range[idx].max) {
			wpa_printf(MSG_DEBUG, "nl80211: Ignore invalid frequency range");
			continue;
		}
		event.freq_range.num++;
	}
	event.freq_range.range = range;

	wpa_supplicant_event(drv->ctx, EVENT_AVOID_FREQUENCIES, &event);

	os_free(range);
}


static enum hostapd_hw_mode get_qca_hw_mode(u8 hw_mode)
{
	switch (hw_mode) {
	case QCA_ACS_MODE_IEEE80211B:
		return HOSTAPD_MODE_IEEE80211B;
	case QCA_ACS_MODE_IEEE80211G:
		return HOSTAPD_MODE_IEEE80211G;
	case QCA_ACS_MODE_IEEE80211A:
		return HOSTAPD_MODE_IEEE80211A;
	case QCA_ACS_MODE_IEEE80211AD:
		return HOSTAPD_MODE_IEEE80211AD;
	case QCA_ACS_MODE_IEEE80211ANY:
		return HOSTAPD_MODE_IEEE80211ANY;
	default:
		return NUM_HOSTAPD_MODES;
	}
}


static unsigned int chan_to_freq(struct wpa_driver_nl80211_data *drv,
				 u8 chan, enum hostapd_hw_mode hw_mode)
{
	if (hw_mode == NUM_HOSTAPD_MODES) {
		/* For drivers that do not report ACS_HW_MODE */
		u16 num_modes, flags;
		struct hostapd_hw_modes *modes;
		u8 dfs_domain;
		int i;

		modes = nl80211_get_hw_feature_data(drv->first_bss, &num_modes,
						    &flags, &dfs_domain);
		if (!modes) {
			wpa_printf(MSG_DEBUG,
				   "nl80211: Fetching hardware mode failed");
			goto try_2_4_or_5;
		}
		if (num_modes == 1)
			hw_mode = modes[0].mode;

		for (i = 0; i < num_modes; i++) {
			os_free(modes[i].channels);
			os_free(modes[i].rates);
		}

		os_free(modes);
	}

	if (hw_mode == HOSTAPD_MODE_IEEE80211AD) {
		if (chan >= 1 && chan <= 6)
			return 56160 + (2160 * chan);
		return 0;
	}

try_2_4_or_5:
	if (chan >= 1 && chan <= 13)
		return 2407 + 5 * chan;
	if (chan == 14)
		return 2484;
	if (chan >= 36 && chan <= 177)
		return 5000 + 5 * chan;

	return 0;
}


static void qca_nl80211_acs_select_ch(struct wpa_driver_nl80211_data *drv,
				   const u8 *data, size_t len)
{
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_ACS_MAX + 1];
	union wpa_event_data event;
	u8 chan;

	wpa_printf(MSG_DEBUG,
		   "nl80211: ACS channel selection vendor event received");

	if (nla_parse(tb, QCA_WLAN_VENDOR_ATTR_ACS_MAX,
		      (struct nlattr *) data, len, NULL) ||
	    (!tb[QCA_WLAN_VENDOR_ATTR_ACS_PRIMARY_FREQUENCY] &&
	     !tb[QCA_WLAN_VENDOR_ATTR_ACS_PRIMARY_CHANNEL]) ||
	    (!tb[QCA_WLAN_VENDOR_ATTR_ACS_SECONDARY_FREQUENCY] &&
	     !tb[QCA_WLAN_VENDOR_ATTR_ACS_SECONDARY_CHANNEL]))
		return;

	os_memset(&event, 0, sizeof(event));
	event.acs_selected_channels.hw_mode = NUM_HOSTAPD_MODES;

	if (tb[QCA_WLAN_VENDOR_ATTR_ACS_HW_MODE]) {
		u8 hw_mode = nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_ACS_HW_MODE]);

		event.acs_selected_channels.hw_mode = get_qca_hw_mode(hw_mode);
		if (event.acs_selected_channels.hw_mode == NUM_HOSTAPD_MODES ||
		    event.acs_selected_channels.hw_mode ==
		    HOSTAPD_MODE_IEEE80211ANY) {
			wpa_printf(MSG_DEBUG,
				   "nl80211: Invalid hw_mode %d in ACS selection event",
				   hw_mode);
			return;
		}
	}

	if (tb[QCA_WLAN_VENDOR_ATTR_ACS_PRIMARY_FREQUENCY]) {
		event.acs_selected_channels.pri_freq = nla_get_u32(
			tb[QCA_WLAN_VENDOR_ATTR_ACS_PRIMARY_FREQUENCY]);
	} else {
		chan = nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_ACS_PRIMARY_CHANNEL]);
		event.acs_selected_channels.pri_freq =
			chan_to_freq(drv, chan,
				     event.acs_selected_channels.hw_mode);
	}

	if (tb[QCA_WLAN_VENDOR_ATTR_ACS_SECONDARY_FREQUENCY]) {
		event.acs_selected_channels.sec_freq = nla_get_u32(
			tb[QCA_WLAN_VENDOR_ATTR_ACS_SECONDARY_FREQUENCY]);
	} else {
		chan = nla_get_u8(
			tb[QCA_WLAN_VENDOR_ATTR_ACS_SECONDARY_CHANNEL]);
		event.acs_selected_channels.sec_freq =
			chan_to_freq(drv, chan,
				     event.acs_selected_channels.hw_mode);
	}

	if (tb[QCA_WLAN_VENDOR_ATTR_ACS_EDMG_CHANNEL])
		event.acs_selected_channels.edmg_channel =
			nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_ACS_EDMG_CHANNEL]);
	if (tb[QCA_WLAN_VENDOR_ATTR_ACS_VHT_SEG0_CENTER_CHANNEL])
		event.acs_selected_channels.vht_seg0_center_ch =
			nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_ACS_VHT_SEG0_CENTER_CHANNEL]);
	if (tb[QCA_WLAN_VENDOR_ATTR_ACS_VHT_SEG1_CENTER_CHANNEL])
		event.acs_selected_channels.vht_seg1_center_ch =
			nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_ACS_VHT_SEG1_CENTER_CHANNEL]);
	if (tb[QCA_WLAN_VENDOR_ATTR_ACS_CHWIDTH])
		event.acs_selected_channels.ch_width =
			nla_get_u16(tb[QCA_WLAN_VENDOR_ATTR_ACS_CHWIDTH]);
	wpa_printf(MSG_INFO,
		   "nl80211: ACS Results: PFreq: %d SFreq: %d BW: %d VHT0: %d VHT1: %d HW_MODE: %d EDMGCH: %d",
		   event.acs_selected_channels.pri_freq,
		   event.acs_selected_channels.sec_freq,
		   event.acs_selected_channels.ch_width,
		   event.acs_selected_channels.vht_seg0_center_ch,
		   event.acs_selected_channels.vht_seg1_center_ch,
		   event.acs_selected_channels.hw_mode,
		   event.acs_selected_channels.edmg_channel);

	/* Ignore ACS channel list check for backwards compatibility */

	wpa_supplicant_event(drv->ctx, EVENT_ACS_CHANNEL_SELECTED, &event);
}


static void qca_nl80211_key_mgmt_auth(struct wpa_driver_nl80211_data *drv,
				      const u8 *data, size_t len)
{
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_MAX + 1];
	u8 *bssid;

	wpa_printf(MSG_DEBUG,
		   "nl80211: Key management roam+auth vendor event received");

	if (nla_parse(tb, QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_MAX,
		      (struct nlattr *) data, len, NULL) ||
	    !tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_BSSID] ||
	    nla_len(tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_BSSID]) != ETH_ALEN ||
	    !tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_REQ_IE] ||
	    !tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_RESP_IE] ||
	    !tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_AUTHORIZED])
		return;

	bssid = nla_data(tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_BSSID]);
	wpa_printf(MSG_DEBUG, "  * roam BSSID " MACSTR, MAC2STR(bssid));

	mlme_event_connect(drv, NL80211_CMD_ROAM, NULL,
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_BSSID],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_REQ_IE],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_RESP_IE],
			   NULL, NULL,
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_AUTHORIZED],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_KEY_REPLAY_CTR],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_PTK_KCK],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_PTK_KEK],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_SUBNET_STATUS],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_FILS_ERP_NEXT_SEQ_NUM],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_PMK],
			   tb[QCA_WLAN_VENDOR_ATTR_ROAM_AUTH_PMKID]);
}


static void
qca_nl80211_key_mgmt_auth_handler(struct wpa_driver_nl80211_data *drv,
				  const u8 *data, size_t len)
{
	if (!drv->roam_indication_done) {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Pending roam indication, delay processing roam+auth vendor event");
		os_get_reltime(&drv->pending_roam_ind_time);

		os_free(drv->pending_roam_data);
		drv->pending_roam_data = os_memdup(data, len);
		if (!drv->pending_roam_data)
			return;
		drv->pending_roam_data_len = len;
		return;
	}
	drv->roam_indication_done = false;
	qca_nl80211_key_mgmt_auth(drv, data, len);
}


static void qca_nl80211_dfs_offload_radar_event(
	struct wpa_driver_nl80211_data *drv, u32 subcmd, u8 *msg, int length)
{
	union wpa_event_data data;
	struct nlattr *tb[NL80211_ATTR_MAX + 1];

	wpa_printf(MSG_DEBUG,
		   "nl80211: DFS offload radar vendor event received");

	if (nla_parse(tb, NL80211_ATTR_MAX,
		      (struct nlattr *) msg, length, NULL))
		return;

	if (!tb[NL80211_ATTR_WIPHY_FREQ]) {
		wpa_printf(MSG_INFO,
			   "nl80211: Error parsing WIPHY_FREQ in FS offload radar vendor event");
		return;
	}

	os_memset(&data, 0, sizeof(data));
	data.dfs_event.freq = nla_get_u32(tb[NL80211_ATTR_WIPHY_FREQ]);

	wpa_printf(MSG_DEBUG, "nl80211: DFS event on freq %d MHz",
		   data.dfs_event.freq);

	/* Check HT params */
	if (tb[NL80211_ATTR_WIPHY_CHANNEL_TYPE]) {
		data.dfs_event.ht_enabled = 1;
		data.dfs_event.chan_offset = 0;

		switch (nla_get_u32(tb[NL80211_ATTR_WIPHY_CHANNEL_TYPE])) {
		case NL80211_CHAN_NO_HT:
			data.dfs_event.ht_enabled = 0;
			break;
		case NL80211_CHAN_HT20:
			break;
		case NL80211_CHAN_HT40PLUS:
			data.dfs_event.chan_offset = 1;
			break;
		case NL80211_CHAN_HT40MINUS:
			data.dfs_event.chan_offset = -1;
			break;
		}
	}

	/* Get VHT params */
	if (tb[NL80211_ATTR_CHANNEL_WIDTH])
		data.dfs_event.chan_width =
			convert2width(nla_get_u32(
					      tb[NL80211_ATTR_CHANNEL_WIDTH]));
	if (tb[NL80211_ATTR_CENTER_FREQ1])
		data.dfs_event.cf1 = nla_get_u32(tb[NL80211_ATTR_CENTER_FREQ1]);
	if (tb[NL80211_ATTR_CENTER_FREQ2])
		data.dfs_event.cf2 = nla_get_u32(tb[NL80211_ATTR_CENTER_FREQ2]);

	wpa_printf(MSG_DEBUG, "nl80211: DFS event on freq %d MHz, ht: %d, "
		    "offset: %d, width: %d, cf1: %dMHz, cf2: %dMHz",
		    data.dfs_event.freq, data.dfs_event.ht_enabled,
		    data.dfs_event.chan_offset, data.dfs_event.chan_width,
		    data.dfs_event.cf1, data.dfs_event.cf2);

	switch (subcmd) {
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_RADAR_DETECTED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_RADAR_DETECTED, &data);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_CAC_STARTED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_CAC_STARTED, &data);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_CAC_FINISHED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_CAC_FINISHED, &data);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_CAC_ABORTED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_CAC_ABORTED, &data);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_CAC_NOP_FINISHED:
		wpa_supplicant_event(drv->ctx, EVENT_DFS_NOP_FINISHED, &data);
		break;
	default:
		wpa_printf(MSG_DEBUG,
			   "nl80211: Unknown DFS offload radar event %d received",
			   subcmd);
		break;
	}
}


static void qca_nl80211_scan_trigger_event(struct wpa_driver_nl80211_data *drv,
					   u8 *data, size_t len)
{
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_SCAN_MAX + 1];
	u64 cookie = 0;
	union wpa_event_data event;
	struct scan_info *info;

	if (nla_parse(tb, QCA_WLAN_VENDOR_ATTR_SCAN_MAX,
		      (struct nlattr *) data, len, NULL) ||
	    !tb[QCA_WLAN_VENDOR_ATTR_SCAN_COOKIE])
		return;

	cookie = nla_get_u64(tb[QCA_WLAN_VENDOR_ATTR_SCAN_COOKIE]);
	if (cookie != drv->vendor_scan_cookie) {
		/* External scan trigger event, ignore */
		return;
	}

	/* Cookie match, own scan */
	os_memset(&event, 0, sizeof(event));
	info = &event.scan_info;
	info->external_scan = 0;
	info->nl_scan_event = 0;

	drv->scan_state = SCAN_STARTED;
	wpa_supplicant_event(drv->ctx, EVENT_SCAN_STARTED, &event);
}


static void send_vendor_scan_event(struct wpa_driver_nl80211_data *drv,
				   int aborted, struct nlattr *tb[],
				   int external_scan)
{
	union wpa_event_data event;
	struct nlattr *nl;
	int rem;
	struct scan_info *info;
	int freqs[MAX_REPORT_FREQS];
	int num_freqs = 0;

	os_memset(&event, 0, sizeof(event));
	info = &event.scan_info;
	info->aborted = aborted;
	info->external_scan = external_scan;

	if (tb[QCA_WLAN_VENDOR_ATTR_SCAN_SSIDS]) {
		nla_for_each_nested(nl,
				    tb[QCA_WLAN_VENDOR_ATTR_SCAN_SSIDS], rem) {
			struct wpa_driver_scan_ssid *s =
				&info->ssids[info->num_ssids];
			s->ssid = nla_data(nl);
			s->ssid_len = nla_len(nl);
			wpa_printf(MSG_DEBUG,
				   "nl80211: Scan probed for SSID '%s'",
				   wpa_ssid_txt(s->ssid, s->ssid_len));
			info->num_ssids++;
			if (info->num_ssids == WPAS_MAX_SCAN_SSIDS)
				break;
		}
	}

	if (tb[QCA_WLAN_VENDOR_ATTR_SCAN_FREQUENCIES]) {
		char msg[500], *pos, *end;
		int res;

		pos = msg;
		end = pos + sizeof(msg);
		*pos = '\0';

		nla_for_each_nested(nl,
				    tb[QCA_WLAN_VENDOR_ATTR_SCAN_FREQUENCIES],
				    rem) {
			freqs[num_freqs] = nla_get_u32(nl);
			res = os_snprintf(pos, end - pos, " %d",
					  freqs[num_freqs]);
			if (!os_snprintf_error(end - pos, res))
				pos += res;
			num_freqs++;
			if (num_freqs == MAX_REPORT_FREQS - 1)
				break;
		}

		info->freqs = freqs;
		info->num_freqs = num_freqs;
		wpa_printf(MSG_DEBUG, "nl80211: Scan included frequencies:%s",
			   msg);
	}
	wpa_supplicant_event(drv->ctx, EVENT_SCAN_RESULTS, &event);
}


static void qca_nl80211_scan_done_event(struct wpa_driver_nl80211_data *drv,
					u8 *data, size_t len)
{
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_SCAN_MAX + 1];
	u64 cookie = 0;
	enum scan_status status;
	int external_scan;

	if (nla_parse(tb, QCA_WLAN_VENDOR_ATTR_SCAN_MAX,
		      (struct nlattr *) data, len, NULL) ||
	    !tb[QCA_WLAN_VENDOR_ATTR_SCAN_STATUS] ||
	    !tb[QCA_WLAN_VENDOR_ATTR_SCAN_COOKIE])
		return;

	status = nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_SCAN_STATUS]);
	if (status >= VENDOR_SCAN_STATUS_MAX)
		return; /* invalid status */

	cookie = nla_get_u64(tb[QCA_WLAN_VENDOR_ATTR_SCAN_COOKIE]);
	if (cookie != drv->vendor_scan_cookie) {
		/* Event from an external scan, get scan results */
		external_scan = 1;
	} else {
		external_scan = 0;
		if (status == VENDOR_SCAN_STATUS_NEW_RESULTS)
			drv->scan_state = SCAN_COMPLETED;
		else
			drv->scan_state = SCAN_ABORTED;

		eloop_cancel_timeout(wpa_driver_nl80211_scan_timeout, drv,
				     drv->ctx);
		drv->vendor_scan_cookie = 0;
		drv->last_scan_cmd = 0;
	}

	send_vendor_scan_event(drv, (status == VENDOR_SCAN_STATUS_ABORTED), tb,
			       external_scan);
}


static void qca_nl80211_p2p_lo_stop_event(struct wpa_driver_nl80211_data *drv,
					  u8 *data, size_t len)
{
	struct nlattr *tb[QCA_WLAN_VENDOR_ATTR_P2P_LISTEN_OFFLOAD_MAX + 1];
	union wpa_event_data event;

	wpa_printf(MSG_DEBUG,
		   "nl80211: P2P listen offload stop vendor event received");

	if (nla_parse(tb, QCA_WLAN_VENDOR_ATTR_P2P_LISTEN_OFFLOAD_MAX,
		      (struct nlattr *) data, len, NULL) ||
	    !tb[QCA_WLAN_VENDOR_ATTR_P2P_LISTEN_OFFLOAD_STOP_REASON])
		return;

	os_memset(&event, 0, sizeof(event));
	event.p2p_lo_stop.reason_code =
		nla_get_u8(tb[QCA_WLAN_VENDOR_ATTR_P2P_LISTEN_OFFLOAD_STOP_REASON]);

	wpa_printf(MSG_DEBUG,
		   "nl80211: P2P Listen offload stop reason: %d",
		   event.p2p_lo_stop.reason_code);
	wpa_supplicant_event(drv->ctx, EVENT_P2P_LO_STOP, &event);
}

#endif /* CONFIG_DRIVER_NL80211_QCA */


static void nl80211_vendor_event_qca(struct wpa_driver_nl80211_data *drv,
				     u32 subcmd, u8 *data, size_t len)
{
	switch (subcmd) {
	case QCA_NL80211_VENDOR_SUBCMD_TEST:
		wpa_hexdump(MSG_DEBUG, "nl80211: QCA test event", data, len);
		break;
#ifdef CONFIG_DRIVER_NL80211_QCA
	case QCA_NL80211_VENDOR_SUBCMD_AVOID_FREQUENCY:
		qca_nl80211_avoid_freq(drv, data, len);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_KEY_MGMT_ROAM_AUTH:
		qca_nl80211_key_mgmt_auth_handler(drv, data, len);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_DO_ACS:
		qca_nl80211_acs_select_ch(drv, data, len);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_CAC_STARTED:
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_CAC_FINISHED:
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_CAC_ABORTED:
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_CAC_NOP_FINISHED:
	case QCA_NL80211_VENDOR_SUBCMD_DFS_OFFLOAD_RADAR_DETECTED:
		qca_nl80211_dfs_offload_radar_event(drv, subcmd, data, len);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_TRIGGER_SCAN:
		qca_nl80211_scan_trigger_event(drv, data, len);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_SCAN_DONE:
		qca_nl80211_scan_done_event(drv, data, len);
		break;
	case QCA_NL80211_VENDOR_SUBCMD_P2P_LISTEN_OFFLOAD_STOP:
		qca_nl80211_p2p_lo_stop_event(drv, data, len);
		break;
#endif /* CONFIG_DRIVER_NL80211_QCA */
	default:
		wpa_printf(MSG_DEBUG,
			   "nl80211: Ignore unsupported QCA vendor event %u",
			   subcmd);
		break;
	}
}


#ifdef CONFIG_DRIVER_NL80211_BRCM

static void brcm_nl80211_acs_select_ch(struct wpa_driver_nl80211_data *drv,
				       const u8 *data, size_t len)
{
	struct nlattr *tb[BRCM_VENDOR_ATTR_ACS_LAST + 1];
	union wpa_event_data event;

	wpa_printf(MSG_DEBUG,
		   "nl80211: BRCM ACS channel selection vendor event received");

	if (nla_parse(tb, BRCM_VENDOR_ATTR_ACS_LAST, (struct nlattr *) data,
		      len, NULL) ||
	    !tb[BRCM_VENDOR_ATTR_ACS_PRIMARY_FREQ] ||
	    !tb[BRCM_VENDOR_ATTR_ACS_SECONDARY_FREQ])
		return;

	os_memset(&event, 0, sizeof(event));
	if (tb[BRCM_VENDOR_ATTR_ACS_PRIMARY_FREQ])
		event.acs_selected_channels.pri_freq =
			nla_get_u32(tb[BRCM_VENDOR_ATTR_ACS_PRIMARY_FREQ]);
	if (tb[BRCM_VENDOR_ATTR_ACS_SECONDARY_FREQ])
		event.acs_selected_channels.sec_freq =
			nla_get_u32(tb[BRCM_VENDOR_ATTR_ACS_SECONDARY_FREQ]);
	if (tb[BRCM_VENDOR_ATTR_ACS_VHT_SEG0_CENTER_CHANNEL])
		event.acs_selected_channels.vht_seg0_center_ch =
			nla_get_u8(tb[BRCM_VENDOR_ATTR_ACS_VHT_SEG0_CENTER_CHANNEL]);
	if (tb[BRCM_VENDOR_ATTR_ACS_VHT_SEG0_CENTER_CHANNEL])
		event.acs_selected_channels.vht_seg1_center_ch =
			nla_get_u8(tb[BRCM_VENDOR_ATTR_ACS_VHT_SEG1_CENTER_CHANNEL]);
	if (tb[BRCM_VENDOR_ATTR_ACS_CHWIDTH])
		event.acs_selected_channels.ch_width =
			nla_get_u16(tb[BRCM_VENDOR_ATTR_ACS_CHWIDTH]);
	if (tb[BRCM_VENDOR_ATTR_ACS_HW_MODE]) {
		event.acs_selected_channels.hw_mode = nla_get_u8(tb[BRCM_VENDOR_ATTR_ACS_HW_MODE]);
		if (event.acs_selected_channels.hw_mode == NUM_HOSTAPD_MODES ||
		    event.acs_selected_channels.hw_mode ==
		    HOSTAPD_MODE_IEEE80211ANY) {
			wpa_printf(MSG_DEBUG,
				   "nl80211: Invalid hw_mode %d in ACS selection event",
				   event.acs_selected_channels.hw_mode);
			return;
		}
	}

	wpa_printf(MSG_DEBUG,
		   "nl80211: ACS Results: PCH: %d SCH: %d BW: %d VHT0: %d VHT1: %d HW_MODE: %d",
		   event.acs_selected_channels.pri_freq,
		   event.acs_selected_channels.sec_freq,
		   event.acs_selected_channels.ch_width,
		   event.acs_selected_channels.vht_seg0_center_ch,
		   event.acs_selected_channels.vht_seg1_center_ch,
		   event.acs_selected_channels.hw_mode);
	wpa_supplicant_event(drv->ctx, EVENT_ACS_CHANNEL_SELECTED, &event);
}


static void nl80211_vendor_event_brcm(struct wpa_driver_nl80211_data *drv,
				      u32 subcmd, u8 *data, size_t len)
{
	wpa_printf(MSG_DEBUG, "nl80211: Got BRCM vendor event %u", subcmd);
	switch (subcmd) {
	case BRCM_VENDOR_EVENT_PRIV_STR:
	case BRCM_VENDOR_EVENT_HANGED:
		/* Dump the event on to the console */
		wpa_msg(NULL, MSG_INFO, "%s", data);
		break;
	case BRCM_VENDOR_EVENT_ACS:
		brcm_nl80211_acs_select_ch(drv, data, len);
		break;
	default:
		wpa_printf(MSG_DEBUG,
			   "%s: Ignore unsupported BRCM vendor event %u",
			   __func__, subcmd);
		break;
	}
}

#endif /* CONFIG_DRIVER_NL80211_BRCM */


static void nl80211_vendor_event(struct wpa_driver_nl80211_data *drv,
				 struct nlattr **tb)
{
	u32 vendor_id, subcmd, wiphy = 0;
	int wiphy_idx;
	u8 *data = NULL;
	size_t len = 0;

	if (!tb[NL80211_ATTR_VENDOR_ID] ||
	    !tb[NL80211_ATTR_VENDOR_SUBCMD])
		return;

	vendor_id = nla_get_u32(tb[NL80211_ATTR_VENDOR_ID]);
	subcmd = nla_get_u32(tb[NL80211_ATTR_VENDOR_SUBCMD]);

	if (tb[NL80211_ATTR_WIPHY])
		wiphy = nla_get_u32(tb[NL80211_ATTR_WIPHY]);

	wpa_printf(MSG_DEBUG, "nl80211: Vendor event: wiphy=%u vendor_id=0x%x subcmd=%u",
		   wiphy, vendor_id, subcmd);

	if (tb[NL80211_ATTR_VENDOR_DATA]) {
		data = nla_data(tb[NL80211_ATTR_VENDOR_DATA]);
		len = nla_len(tb[NL80211_ATTR_VENDOR_DATA]);
		wpa_hexdump(MSG_MSGDUMP, "nl80211: Vendor data", data, len);
	}

	wiphy_idx = nl80211_get_wiphy_index(drv->first_bss);
	if (wiphy_idx >= 0 && wiphy_idx != (int) wiphy) {
		wpa_printf(MSG_DEBUG, "nl80211: Ignore vendor event for foreign wiphy %u (own: %d)",
			   wiphy, wiphy_idx);
		return;
	}

#ifdef ANDROID
#ifdef ANDROID_LIB_EVENT
       wpa_driver_nl80211_driver_event(drv, vendor_id, subcmd, data, len);
#endif /* ANDROID_LIB_EVENT */
#endif /* ANDROID */

	switch (vendor_id) {
	case OUI_QCA:
		nl80211_vendor_event_qca(drv, subcmd, data, len);
		break;
#ifdef CONFIG_DRIVER_NL80211_BRCM
	case OUI_BRCM:
		nl80211_vendor_event_brcm(drv, subcmd, data, len);
		break;
#endif /* CONFIG_DRIVER_NL80211_BRCM */
	default:
		wpa_printf(MSG_DEBUG, "nl80211: Ignore unsupported vendor event");
		break;
	}
}

#ifdef CFG_INTERFACE
#if 0
static void nl80211_reg_change_event_cfg(struct wpa_driver_nl80211_data *drv,
					 struct nrf_wifi_event_regulatory_change *reg_chng)
{
	union wpa_event_data data;
	int init;
	int type;

	wpa_printf(MSG_DEBUG, "nl80211: Regulatory domain change");

	os_memset(&data, 0, sizeof(data));
	init = reg_chng->intr;
	wpa_printf(MSG_DEBUG, " * initiator=%d", init);
	switch (init) {
	case NRF_WIFI_REGDOM_SET_BY_CORE:
		data.channel_list_changed.initiator = REGDOM_SET_BY_CORE;
		break;
	case NRF_WIFI_REGDOM_SET_BY_USER:
		data.channel_list_changed.initiator = REGDOM_SET_BY_USER;
		break;
	case NRF_WIFI_REGDOM_SET_BY_DRIVER:
		data.channel_list_changed.initiator = REGDOM_SET_BY_DRIVER;
		break;
	case NRF_WIFI_REGDOM_SET_BY_COUNTRY_IE:
		data.channel_list_changed.initiator = REGDOM_SET_BY_COUNTRY_IE;
		break;
	}

	type = reg_chng->regulatory_type;
	wpa_printf(MSG_DEBUG, " * type=%d", type);
	switch (type) {
	case NRF_WIFI_REGDOM_TYPE_COUNTRY:
		data.channel_list_changed.type = REGDOM_TYPE_COUNTRY;
		break;
	case NRF_WIFI_REGDOM_TYPE_WORLD:
		data.channel_list_changed.type = REGDOM_TYPE_WORLD;
		break;
	case NRF_WIFI_REGDOM_TYPE_CUSTOM_WORLD:
		data.channel_list_changed.type =
			REGDOM_TYPE_CUSTOM_WORLD;
		break;
	case NRF_WIFI_REGDOM_TYPE_INTERSECTION:
		data.channel_list_changed.type =
			REGDOM_TYPE_INTERSECTION;
		break;
	}

	if (reg_chng->nrf_wifi_alpha2) {
		os_strlcpy(data.channel_list_changed.alpha2,
			   reg_chng->nrf_wifi_alpha2,
			   sizeof(data.channel_list_changed.alpha2));
		wpa_printf(MSG_DEBUG, " * alpha2=%s",
			   data.channel_list_changed.alpha2);
	}

	wpa_supplicant_event(drv->ctx, EVENT_CHANNEL_LIST_CHANGED, &data);
}
#endif
#endif

static void nl80211_reg_change_event(struct wpa_driver_nl80211_data *drv,
				     struct nlattr *tb[])
{
	union wpa_event_data data;
	enum nl80211_reg_initiator init;

	wpa_printf(MSG_DEBUG, "nl80211: Regulatory domain change");

	if (tb[NL80211_ATTR_REG_INITIATOR] == NULL)
		return;

	os_memset(&data, 0, sizeof(data));
	init = nla_get_u8(tb[NL80211_ATTR_REG_INITIATOR]);
	wpa_printf(MSG_DEBUG, " * initiator=%d", init);
	switch (init) {
	case NL80211_REGDOM_SET_BY_CORE:
		data.channel_list_changed.initiator = REGDOM_SET_BY_CORE;
		break;
	case NL80211_REGDOM_SET_BY_USER:
		data.channel_list_changed.initiator = REGDOM_SET_BY_USER;
		break;
	case NL80211_REGDOM_SET_BY_DRIVER:
		data.channel_list_changed.initiator = REGDOM_SET_BY_DRIVER;
		break;
	case NL80211_REGDOM_SET_BY_COUNTRY_IE:
		data.channel_list_changed.initiator = REGDOM_SET_BY_COUNTRY_IE;
		break;
	}

	if (tb[NL80211_ATTR_REG_TYPE]) {
		enum nl80211_reg_type type;
		type = nla_get_u8(tb[NL80211_ATTR_REG_TYPE]);
		wpa_printf(MSG_DEBUG, " * type=%d", type);
		switch (type) {
		case NL80211_REGDOM_TYPE_COUNTRY:
			data.channel_list_changed.type = REGDOM_TYPE_COUNTRY;
			break;
		case NL80211_REGDOM_TYPE_WORLD:
			data.channel_list_changed.type = REGDOM_TYPE_WORLD;
			break;
		case NL80211_REGDOM_TYPE_CUSTOM_WORLD:
			data.channel_list_changed.type =
				REGDOM_TYPE_CUSTOM_WORLD;
			break;
		case NL80211_REGDOM_TYPE_INTERSECTION:
			data.channel_list_changed.type =
				REGDOM_TYPE_INTERSECTION;
			break;
		}
	}

	if (tb[NL80211_ATTR_REG_ALPHA2]) {
		os_strlcpy(data.channel_list_changed.alpha2,
			   nla_get_string(tb[NL80211_ATTR_REG_ALPHA2]),
			   sizeof(data.channel_list_changed.alpha2));
		wpa_printf(MSG_DEBUG, " * alpha2=%s",
			   data.channel_list_changed.alpha2);
	}

	wpa_supplicant_event(drv->ctx, EVENT_CHANNEL_LIST_CHANGED, &data);
}


static void nl80211_dump_freq(const char *title, struct nlattr *nl_freq)
{
	static struct nla_policy freq_policy[NL80211_FREQUENCY_ATTR_MAX + 1] = {
		[NL80211_FREQUENCY_ATTR_FREQ] = { .type = NLA_U32 },
		[NL80211_FREQUENCY_ATTR_DISABLED] = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_NO_IR] = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_RADAR] = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_MAX_TX_POWER] = { .type = NLA_U32 },
	};
	struct nlattr *tb[NL80211_FREQUENCY_ATTR_MAX + 1];
	u32 freq = 0, max_tx_power = 0;

	nla_parse(tb, NL80211_FREQUENCY_ATTR_MAX,
		  nla_data(nl_freq), nla_len(nl_freq), freq_policy);

	if (tb[NL80211_FREQUENCY_ATTR_FREQ])
		freq = nla_get_u32(tb[NL80211_FREQUENCY_ATTR_FREQ]);
	if (tb[NL80211_FREQUENCY_ATTR_MAX_TX_POWER])
		max_tx_power =
			nla_get_u32(tb[NL80211_FREQUENCY_ATTR_MAX_TX_POWER]);

	wpa_printf(MSG_DEBUG,
		   "nl80211: Channel (%s): freq=%u max_tx_power=%u%s%s%s",
		   title, freq, max_tx_power,
		   tb[NL80211_FREQUENCY_ATTR_DISABLED] ? " disabled" : "",
		   tb[NL80211_FREQUENCY_ATTR_NO_IR] ? " no-IR" : "",
		   tb[NL80211_FREQUENCY_ATTR_RADAR] ? " radar" : "");
}

#ifdef CFG_INTERFACE
#if 0
static void nl80211_reg_beacon_hint_event_cfg(struct wpa_driver_nl80211_data *drv,
					   struct nrf_wifi_event_send_beacon_hint *bcn_hint)
{
	union wpa_event_data data;
	u32 freq = 0, max_tx_power = 0;

	wpa_printf(MSG_DEBUG, "nl80211: Regulatory beacon hint");
	os_memset(&data, 0, sizeof(data));
	data.channel_list_changed.initiator = REGDOM_BEACON_HINT;
	
	if(&bcn_hint->channel_before) 
	{
                freq = bcn_hint->channel_before.center_frequency;
                max_tx_power =  bcn_hint->channel_before.nrf_wifi_max_power;

        	wpa_printf(MSG_DEBUG,
                   "nl80211: Channel (%s): freq=%u max_tx_power=%u%s%s%s",
                   "before", freq, max_tx_power,
                   (bcn_hint->channel_before.nrf_wifi_flags & NRF_WIFI_CHAN_FLAG_FREQUENCY_DISABLED) ? " disabled" : "",
                   (bcn_hint->channel_before.nrf_wifi_flags & NRF_WIFI_CHAN_FLAG_FREQUENCY_ATTR_NO_IR) ? " no-IR" : "",
                   (bcn_hint->channel_before.nrf_wifi_flags & NRF_WIFI_CHAN_FLAG_FREQUENCY_ATTR_RADAR) ? " radar" : "");
	}
	if (&bcn_hint->channel_after) 
	{
                freq = bcn_hint->channel_after.center_frequency;
                max_tx_power =  bcn_hint->channel_after.nrf_wifi_max_power;

        	wpa_printf(MSG_DEBUG,
                   "nl80211: Channel (%s): freq=%u max_tx_power=%u%s%s%s",
                   "after", freq, max_tx_power,
                   (bcn_hint->channel_after.nrf_wifi_flags & NRF_WIFI_CHAN_FLAG_FREQUENCY_DISABLED) ? " disabled" : "",
                   (bcn_hint->channel_after.nrf_wifi_flags & NRF_WIFI_CHAN_FLAG_FREQUENCY_ATTR_NO_IR) ? " no-IR" : "",
                   (bcn_hint->channel_after.nrf_wifi_flags & NRF_WIFI_CHAN_FLAG_FREQUENCY_ATTR_RADAR) ? " radar" : "");
	}
	wpa_supplicant_event(drv->ctx, EVENT_CHANNEL_LIST_CHANGED, &data);
}
#endif
#endif

static void nl80211_reg_beacon_hint_event(struct wpa_driver_nl80211_data *drv,
					   struct nlattr *tb[])
{
	union wpa_event_data data;

	wpa_printf(MSG_DEBUG, "nl80211: Regulatory beacon hint");
	os_memset(&data, 0, sizeof(data));
	data.channel_list_changed.initiator = REGDOM_BEACON_HINT;

	if (tb[NL80211_ATTR_FREQ_BEFORE])
		nl80211_dump_freq("before", tb[NL80211_ATTR_FREQ_BEFORE]);
	if (tb[NL80211_ATTR_FREQ_AFTER])
		nl80211_dump_freq("after", tb[NL80211_ATTR_FREQ_AFTER]);

	wpa_supplicant_event(drv->ctx, EVENT_CHANNEL_LIST_CHANGED, &data);
}


static void nl80211_external_auth(struct wpa_driver_nl80211_data *drv,
				  struct nlattr **tb)
{
	union wpa_event_data event;
	enum nl80211_external_auth_action act;

	if (!tb[NL80211_ATTR_AKM_SUITES] ||
	    !tb[NL80211_ATTR_EXTERNAL_AUTH_ACTION] ||
	    !tb[NL80211_ATTR_BSSID] ||
	    !tb[NL80211_ATTR_SSID])
		return;

	os_memset(&event, 0, sizeof(event));
	act = nla_get_u32(tb[NL80211_ATTR_EXTERNAL_AUTH_ACTION]);
	switch (act) {
	case NL80211_EXTERNAL_AUTH_START:
		event.external_auth.action = EXT_AUTH_START;
		break;
	case NL80211_EXTERNAL_AUTH_ABORT:
		event.external_auth.action = EXT_AUTH_ABORT;
		break;
	default:
		return;
	}

	event.external_auth.key_mgmt_suite =
		nla_get_u32(tb[NL80211_ATTR_AKM_SUITES]);

	event.external_auth.ssid_len = nla_len(tb[NL80211_ATTR_SSID]);
	if (event.external_auth.ssid_len > SSID_MAX_LEN)
		return;
	event.external_auth.ssid = nla_data(tb[NL80211_ATTR_SSID]);

	event.external_auth.bssid = nla_data(tb[NL80211_ATTR_BSSID]);

	wpa_printf(MSG_DEBUG,
		   "nl80211: External auth action: %u, AKM: 0x%x",
		   event.external_auth.action,
		   event.external_auth.key_mgmt_suite);
	wpa_supplicant_event(drv->ctx, EVENT_EXTERNAL_AUTH, &event);
}


static void nl80211_port_authorized(struct wpa_driver_nl80211_data *drv,
				    struct nlattr **tb)
{
	const u8 *addr;

	if (!tb[NL80211_ATTR_MAC] ||
	    nla_len(tb[NL80211_ATTR_MAC]) != ETH_ALEN) {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Ignore port authorized event without BSSID");
		return;
	}

	addr = nla_data(tb[NL80211_ATTR_MAC]);
	if (os_memcmp(addr, drv->bssid, ETH_ALEN) != 0) {
		wpa_printf(MSG_DEBUG,
			   "nl80211: Ignore port authorized event for " MACSTR
			   " (not the currently connected BSSID " MACSTR ")",
			   MAC2STR(addr), MAC2STR(drv->bssid));
		return;
	}

	wpa_supplicant_event(drv->ctx, EVENT_PORT_AUTHORIZED, NULL);
}


static void nl80211_sta_opmode_change_event(struct wpa_driver_nl80211_data *drv,
					    struct nlattr **tb)
{
	union wpa_event_data ed;
	u8 smps_mode, max_bw;

	if (!tb[NL80211_ATTR_MAC] ||
	    (!tb[NL80211_ATTR_CHANNEL_WIDTH] &&
	     !tb[NL80211_ATTR_SMPS_MODE] &&
	     !tb[NL80211_ATTR_NSS]))
		return;

	ed.sta_opmode.smps_mode = SMPS_INVALID;
	ed.sta_opmode.chan_width = CHAN_WIDTH_UNKNOWN;
	ed.sta_opmode.rx_nss = 0xff;
	ed.sta_opmode.addr = nla_data(tb[NL80211_ATTR_MAC]);

	if (tb[NL80211_ATTR_SMPS_MODE]) {
		smps_mode = nla_get_u8(tb[NL80211_ATTR_SMPS_MODE]);
		switch (smps_mode) {
		case NL80211_SMPS_OFF:
			ed.sta_opmode.smps_mode = SMPS_OFF;
			break;
		case NL80211_SMPS_STATIC:
			ed.sta_opmode.smps_mode = SMPS_STATIC;
			break;
		case NL80211_SMPS_DYNAMIC:
			ed.sta_opmode.smps_mode = SMPS_DYNAMIC;
			break;
		default:
			ed.sta_opmode.smps_mode = SMPS_INVALID;
			break;
		}
	}

	if (tb[NL80211_ATTR_CHANNEL_WIDTH]) {
		max_bw = nla_get_u32(tb[NL80211_ATTR_CHANNEL_WIDTH]);
		switch (max_bw) {
		case NL80211_CHAN_WIDTH_20_NOHT:
			ed.sta_opmode.chan_width = CHAN_WIDTH_20_NOHT;
			break;
		case NL80211_CHAN_WIDTH_20:
			ed.sta_opmode.chan_width = CHAN_WIDTH_20;
			break;
		case NL80211_CHAN_WIDTH_40:
			ed.sta_opmode.chan_width = CHAN_WIDTH_40;
			break;
		case NL80211_CHAN_WIDTH_80:
			ed.sta_opmode.chan_width = CHAN_WIDTH_80;
			break;
		case NL80211_CHAN_WIDTH_80P80:
			ed.sta_opmode.chan_width = CHAN_WIDTH_80P80;
			break;
		case NL80211_CHAN_WIDTH_160:
			ed.sta_opmode.chan_width = CHAN_WIDTH_160;
			break;
		default:
			ed.sta_opmode.chan_width = CHAN_WIDTH_UNKNOWN;
			break;

		}
	}

	if (tb[NL80211_ATTR_NSS])
		ed.sta_opmode.rx_nss = nla_get_u8(tb[NL80211_ATTR_NSS]);

	wpa_supplicant_event(drv->ctx, EVENT_STATION_OPMODE_CHANGED, &ed);
}


static void nl80211_control_port_frame(struct wpa_driver_nl80211_data *drv,
				       struct nlattr **tb)
{
	u8 *src_addr;
	u16 ethertype;

	if (!tb[NL80211_ATTR_MAC] ||
	    !tb[NL80211_ATTR_FRAME] ||
	    !tb[NL80211_ATTR_CONTROL_PORT_ETHERTYPE])
		return;

	src_addr = nla_data(tb[NL80211_ATTR_MAC]);
	ethertype = nla_get_u16(tb[NL80211_ATTR_CONTROL_PORT_ETHERTYPE]);

	switch (ethertype) {
	case ETH_P_RSN_PREAUTH:
		wpa_printf(MSG_INFO, "nl80211: Got pre-auth frame from "
			   MACSTR " over control port unexpectedly",
			   MAC2STR(src_addr));
		break;
	case ETH_P_PAE:
		drv_event_eapol_rx(drv->ctx, src_addr,
				   nla_data(tb[NL80211_ATTR_FRAME]),
				   nla_len(tb[NL80211_ATTR_FRAME]));
		break;
	default:
		wpa_printf(MSG_INFO, "nl80211: Unxpected ethertype 0x%04x from "
			   MACSTR " over control port",
			   ethertype, MAC2STR(src_addr));
		break;
	}
}


static void
nl80211_control_port_frame_tx_status(struct wpa_driver_nl80211_data *drv,
				     const u8 *frame, size_t len,
				     struct nlattr *ack, struct nlattr *cookie)
{
	union wpa_event_data event;

	if (!cookie || len < ETH_HLEN)
		return;

	wpa_printf(MSG_DEBUG,
		   "nl80211: Control port TX status (ack=%d), cookie=%llu",
		   ack != NULL, (long long unsigned int) nla_get_u64(cookie));

	os_memset(&event, 0, sizeof(event));
	event.eapol_tx_status.dst = frame;
	event.eapol_tx_status.data = frame + ETH_HLEN;
	event.eapol_tx_status.data_len = len - ETH_HLEN;
	event.eapol_tx_status.ack = ack != NULL;
	wpa_supplicant_event(drv->ctx, EVENT_EAPOL_TX_STATUS, &event);
}

#ifdef CFG_INTERFACE
static void cfg_do_process_drv_event(struct i802_bss *bss,
					int cmd,
					char* msg,
					int len)
{
	struct wpa_driver_nl80211_data *drv = bss->drv;
	struct nrf_wifi_umac_event_mlme *event;
	struct nrf_wifi_umac_event_trigger_scan *scan_event, *scan_done_event;
	int external_scan_event = 0;

	wpa_printf(MSG_DEBUG, "nl80211: Drv Event %d (%s) received for %s",
		   cmd, nl80211_command_to_string(cmd), bss->ifname);
	
	switch (cmd) {
	//DEEPAK
	case NRF_WIFI_UMAC_EVENT_FRAME:
    case NRF_WIFI_UMAC_EVENT_FRAME_TX_STATUS:
        cfg_mlme_event(bss, cmd, msg, len);
        break;
	case NRF_WIFI_UMAC_EVENT_REMAIN_ON_CHANNEL:
		cfg_mlme_event_remain_on_channel(drv, 0, msg);
		break;
	case NRF_WIFI_UMAC_EVENT_CANCEL_REMAIN_ON_CHANNEL:
		cfg_mlme_event_remain_on_channel(drv, 1, msg);
		break;

	case NRF_WIFI_UMAC_EVENT_AUTHENTICATE:
		cfg_mlme_event(bss, NL80211_CMD_AUTHENTICATE, msg, len);
	break;
	case NRF_WIFI_UMAC_EVENT_ASSOCIATE:
		cfg_mlme_event(bss, NL80211_CMD_ASSOCIATE, msg, len);
	break;
	case NRF_WIFI_UMAC_EVENT_DEAUTHENTICATE:
		cfg_mlme_event(bss, NL80211_CMD_DEAUTHENTICATE, msg, len);
	break;
	case NRF_WIFI_UMAC_EVENT_DISASSOCIATE:
		cfg_mlme_event(bss, NL80211_CMD_DISASSOCIATE, msg, len);
	break;
	case NRF_WIFI_UMAC_EVENT_UNPROT_DEAUTHENTICATE:
		cfg_mlme_event(bss, NL80211_CMD_UNPROT_DEAUTHENTICATE, msg, len);
	break;
	case NRF_WIFI_UMAC_EVENT_UNPROT_DISASSOCIATE:
		cfg_mlme_event(bss, NL80211_CMD_UNPROT_DEAUTHENTICATE, msg, len);
	break;
	case NRF_WIFI_UMAC_EVENT_TRIGGER_SCAN_START:
		scan_event = (struct nrf_wifi_umac_event_trigger_scan*)msg;
		wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: Scan trigger");
		drv->scan_state = SCAN_STARTED;
		if (drv->scan_for_auth) {
			/*
			 * Cannot indicate EVENT_SCAN_STARTED here since we skip
			 * EVENT_SCAN_RESULTS in scan_for_auth case and the
			 * upper layer implementation could get confused about
			 * scanning state.
			 */
			wpa_printf(MSG_DEBUG, "nl80211: Do not indicate scan-start event due to internal scan_for_auth");
			break;
		}
		wpa_supplicant_event(drv->ctx, EVENT_SCAN_STARTED, NULL);
	break;
	case NRF_WIFI_UMAC_EVENT_SCAN_DONE:
		if (drv->last_scan_cmd != NL80211_CMD_VENDOR)
                        drv->scan_state = SCAN_COMPLETED;
                drv->scan_complete_events = 1;
                if (drv->last_scan_cmd == NL80211_CMD_TRIGGER_SCAN) {
                        eloop_cancel_timeout(wpa_driver_nl80211_scan_timeout,
                                             drv, drv->ctx);
                        drv->last_scan_cmd = 0;
                } else {
                        external_scan_event = 1;
                }
		send_scan_event_cfg(drv, 0, msg, external_scan_event); 
	break;
	case NRF_WIFI_UMAC_EVENT_BEACON_HINT:
		//nl80211_reg_beacon_hint_event_cfg(drv, msg);
	break;
	case NRF_WIFI_UMAC_EVENT_REG_CHANGE:
	case NRF_WIFI_UMAC_EVENT_WIPHY_REG_CHANGE:
		//nl80211_reg_change_event_cfg(drv, msg);
	break;
	default:
		wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: Ignored unknown event "
			"(cmd=%d)", cmd);
	break;
	}
}
#endif

static void do_process_drv_event(struct i802_bss *bss, int cmd,
				 struct nlattr **tb)
{
	struct wpa_driver_nl80211_data *drv = bss->drv;
	int external_scan_event = 0;
	struct nlattr *frame = tb[NL80211_ATTR_FRAME];

	wpa_printf(MSG_DEBUG, "nl80211: Drv Event %d (%s) received for %s",
		   cmd, nl80211_command_to_string(cmd), bss->ifname);

#ifdef CONFIG_DRIVER_NL80211_QCA
	if (cmd == NL80211_CMD_ROAM &&
	    (drv->capa.flags & WPA_DRIVER_FLAGS_KEY_MGMT_OFFLOAD)) {
		if (drv->pending_roam_data) {
			struct os_reltime now, age;

			os_get_reltime(&now);
			os_reltime_sub(&now, &drv->pending_roam_ind_time, &age);
			if (age.sec == 0 && age.usec < 100000) {
				wpa_printf(MSG_DEBUG,
					   "nl80211: Process pending roam+auth vendor event");
				qca_nl80211_key_mgmt_auth(
					drv, drv->pending_roam_data,
					drv->pending_roam_data_len);
			}
			os_free(drv->pending_roam_data);
			drv->pending_roam_data = NULL;
			return;
		}
		/*
		 * Device will use roam+auth vendor event to indicate
		 * roaming, so ignore the regular roam event.
		 */
		drv->roam_indication_done = true;
		wpa_printf(MSG_DEBUG,
			   "nl80211: Ignore roam event (cmd=%d), device will use vendor event roam+auth",
			   cmd);
		return;
	}
#endif /* CONFIG_DRIVER_NL80211_QCA */

	if (drv->ap_scan_as_station != NL80211_IFTYPE_UNSPECIFIED &&
	    (cmd == NL80211_CMD_NEW_SCAN_RESULTS ||
	     cmd == NL80211_CMD_SCAN_ABORTED))
		nl80211_restore_ap_mode(bss);

	switch (cmd) {
	case NL80211_CMD_TRIGGER_SCAN:
		wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: Scan trigger");
		drv->scan_state = SCAN_STARTED;
		if (drv->scan_for_auth) {
			/*
			 * Cannot indicate EVENT_SCAN_STARTED here since we skip
			 * EVENT_SCAN_RESULTS in scan_for_auth case and the
			 * upper layer implementation could get confused about
			 * scanning state.
			 */
			wpa_printf(MSG_DEBUG, "nl80211: Do not indicate scan-start event due to internal scan_for_auth");
			break;
		}
		wpa_supplicant_event(drv->ctx, EVENT_SCAN_STARTED, NULL);
		break;
	case NL80211_CMD_START_SCHED_SCAN:
		wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: Sched scan started");
		drv->scan_state = SCHED_SCAN_STARTED;
		break;
	case NL80211_CMD_SCHED_SCAN_STOPPED:
		wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: Sched scan stopped");
		drv->scan_state = SCHED_SCAN_STOPPED;
		wpa_supplicant_event(drv->ctx, EVENT_SCHED_SCAN_STOPPED, NULL);
		break;
	case NL80211_CMD_NEW_SCAN_RESULTS:
		wpa_dbg(drv->ctx, MSG_DEBUG,
			"nl80211: New scan results available");
		if (drv->last_scan_cmd != NL80211_CMD_VENDOR)
			drv->scan_state = SCAN_COMPLETED;
		drv->scan_complete_events = 1;
		if (drv->last_scan_cmd == NL80211_CMD_TRIGGER_SCAN) {
			eloop_cancel_timeout(wpa_driver_nl80211_scan_timeout,
					     drv, drv->ctx);
			drv->last_scan_cmd = 0;
		} else {
			external_scan_event = 1;
		}
		send_scan_event(drv, 0, tb, external_scan_event);
		break;
	case NL80211_CMD_SCHED_SCAN_RESULTS:
		wpa_dbg(drv->ctx, MSG_DEBUG,
			"nl80211: New sched scan results available");
		drv->scan_state = SCHED_SCAN_RESULTS;
		send_scan_event(drv, 0, tb, 0);
		break;
	case NL80211_CMD_SCAN_ABORTED:
		wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: Scan aborted");
		if (drv->last_scan_cmd != NL80211_CMD_VENDOR)
			drv->scan_state = SCAN_ABORTED;
		if (drv->last_scan_cmd == NL80211_CMD_TRIGGER_SCAN) {
			/*
			 * Need to indicate that scan results are available in
			 * order not to make wpa_supplicant stop its scanning.
			 */
			eloop_cancel_timeout(wpa_driver_nl80211_scan_timeout,
					     drv, drv->ctx);
			drv->last_scan_cmd = 0;
		} else {
			external_scan_event = 1;
		}
		send_scan_event(drv, 1, tb, external_scan_event);
		break;
	case NL80211_CMD_AUTHENTICATE:
	case NL80211_CMD_ASSOCIATE:
	case NL80211_CMD_DEAUTHENTICATE:
	case NL80211_CMD_DISASSOCIATE:
	case NL80211_CMD_FRAME_TX_STATUS:
	case NL80211_CMD_UNPROT_DEAUTHENTICATE:
	case NL80211_CMD_UNPROT_DISASSOCIATE:
		mlme_event(bss, cmd, tb[NL80211_ATTR_FRAME],
			   tb[NL80211_ATTR_MAC], tb[NL80211_ATTR_TIMED_OUT],
			   tb[NL80211_ATTR_WIPHY_FREQ], tb[NL80211_ATTR_ACK],
			   tb[NL80211_ATTR_COOKIE],
			   tb[NL80211_ATTR_RX_SIGNAL_DBM],
			   tb[NL80211_ATTR_STA_WME],
			   tb[NL80211_ATTR_REQ_IE]);
		break;
	case NL80211_CMD_CONNECT:
	case NL80211_CMD_ROAM:
		mlme_event_connect(drv, cmd,
				   tb[NL80211_ATTR_STATUS_CODE],
				   tb[NL80211_ATTR_MAC],
				   tb[NL80211_ATTR_REQ_IE],
				   tb[NL80211_ATTR_RESP_IE],
				   tb[NL80211_ATTR_TIMED_OUT],
				   tb[NL80211_ATTR_TIMEOUT_REASON],
				   NULL, NULL, NULL,
				   tb[NL80211_ATTR_FILS_KEK],
				   NULL,
				   tb[NL80211_ATTR_FILS_ERP_NEXT_SEQ_NUM],
				   tb[NL80211_ATTR_PMK],
				   tb[NL80211_ATTR_PMKID]);
		break;
	case NL80211_CMD_CH_SWITCH_STARTED_NOTIFY:
		mlme_event_ch_switch(drv,
				     tb[NL80211_ATTR_IFINDEX],
				     tb[NL80211_ATTR_WIPHY_FREQ],
				     tb[NL80211_ATTR_WIPHY_CHANNEL_TYPE],
				     tb[NL80211_ATTR_CHANNEL_WIDTH],
				     tb[NL80211_ATTR_CENTER_FREQ1],
				     tb[NL80211_ATTR_CENTER_FREQ2],
				     0);
		break;
	case NL80211_CMD_CH_SWITCH_NOTIFY:
		mlme_event_ch_switch(drv,
				     tb[NL80211_ATTR_IFINDEX],
				     tb[NL80211_ATTR_WIPHY_FREQ],
				     tb[NL80211_ATTR_WIPHY_CHANNEL_TYPE],
				     tb[NL80211_ATTR_CHANNEL_WIDTH],
				     tb[NL80211_ATTR_CENTER_FREQ1],
				     tb[NL80211_ATTR_CENTER_FREQ2],
				     1);
		break;
	case NL80211_CMD_DISCONNECT:
		mlme_event_disconnect(drv, tb[NL80211_ATTR_REASON_CODE],
				      tb[NL80211_ATTR_MAC],
				      tb[NL80211_ATTR_DISCONNECTED_BY_AP]);
		break;
	case NL80211_CMD_MICHAEL_MIC_FAILURE:
		mlme_event_michael_mic_failure(bss, tb);
		break;
	case NL80211_CMD_JOIN_IBSS:
		mlme_event_join_ibss(drv, tb);
		break;
	case NL80211_CMD_REMAIN_ON_CHANNEL:
		mlme_event_remain_on_channel(drv, 0, tb);
		break;
	case NL80211_CMD_CANCEL_REMAIN_ON_CHANNEL:
		mlme_event_remain_on_channel(drv, 1, tb);
		break;
	case NL80211_CMD_NOTIFY_CQM:
		nl80211_cqm_event(drv, tb);
		break;
	case NL80211_CMD_REG_CHANGE:
	case NL80211_CMD_WIPHY_REG_CHANGE:
		nl80211_reg_change_event(drv, tb);
		break;
	case NL80211_CMD_REG_BEACON_HINT:
		nl80211_reg_beacon_hint_event(drv, tb);
		break;
	case NL80211_CMD_NEW_STATION:
		nl80211_new_station_event(drv, bss, tb);
		break;
	case NL80211_CMD_DEL_STATION:
		nl80211_del_station_event(drv, bss, tb);
		break;
	case NL80211_CMD_SET_REKEY_OFFLOAD:
		nl80211_rekey_offload_event(drv, tb);
		break;
	case NL80211_CMD_PMKSA_CANDIDATE:
		nl80211_pmksa_candidate_event(drv, tb);
		break;
	case NL80211_CMD_PROBE_CLIENT:
		nl80211_client_probe_event(drv, tb);
		break;
	case NL80211_CMD_TDLS_OPER:
		nl80211_tdls_oper_event(drv, tb);
		break;
	case NL80211_CMD_CONN_FAILED:
		nl80211_connect_failed_event(drv, tb);
		break;
	case NL80211_CMD_FT_EVENT:
		mlme_event_ft_event(drv, tb);
		break;
	case NL80211_CMD_RADAR_DETECT:
		nl80211_radar_event(drv, tb);
		break;
	case NL80211_CMD_STOP_AP:
		nl80211_stop_ap(drv, tb);
		break;
	case NL80211_CMD_VENDOR:
		nl80211_vendor_event(drv, tb);
		break;
	case NL80211_CMD_NEW_PEER_CANDIDATE:
		nl80211_new_peer_candidate(drv, tb);
		break;
	case NL80211_CMD_PORT_AUTHORIZED:
		nl80211_port_authorized(drv, tb);
		break;
	case NL80211_CMD_STA_OPMODE_CHANGED:
		nl80211_sta_opmode_change_event(drv, tb);
		break;
	case NL80211_CMD_UPDATE_OWE_INFO:
		mlme_event_dh_event(drv, bss, tb);
		break;
	case NL80211_CMD_UNPROT_BEACON:
		if (frame)
			mlme_event_unprot_beacon(drv, nla_data(frame),
						 nla_len(frame));
		break;
	case NL80211_CMD_CONTROL_PORT_FRAME_TX_STATUS:
		if (!frame)
			break;
		nl80211_control_port_frame_tx_status(drv,
						     nla_data(frame),
						     nla_len(frame),
						     tb[NL80211_ATTR_ACK],
						     tb[NL80211_ATTR_COOKIE]);
		break;
	default:
		wpa_dbg(drv->ctx, MSG_DEBUG, "nl80211: Ignored unknown event "
			"(cmd=%d)", cmd);
		break;
	}
}

#ifdef CFG_INTERFACE
int cfg_process_global_event(void *msg, int len, void *arg)
{
	struct nl80211_global *global = arg;
	struct wpa_driver_nl80211_data *drv, *tmp;
	int ifidx = -1, wiphy_idx = -1, wiphy_idx_rx = -1;
	struct i802_bss *bss;
	u64 wdev_id = 0;
	int wdev_id_set = 0;
	int wiphy_idx_set = 0;

	
	int cmd_evnt = ((struct nrf_wifi_umac_hdr *)msg)->cmd_evnt;
//	ifidx  = global->cfg_ifindex;

	unsigned int valid_fields = ((struct nrf_wifi_umac_hdr *)msg)->ids.valid_fields;

	if (valid_fields & NRF_WIFI_INDEX_IDS_IFINDEX_VALID) {
		ifidx = ((struct nrf_wifi_umac_hdr *)msg)->ids.ifaceindex;
	} else if (valid_fields & NRF_WIFI_INDEX_IDS_WDEV_ID_VALID) {
		wdev_id  = ((struct nrf_wifi_umac_hdr *)msg)->ids.wdev_id;
		wdev_id_set = 1;
	} else if (valid_fields & NRF_WIFI_INDEX_IDS_WIPHY_IDX_VALID) {
		wiphy_idx_rx  = ((struct nrf_wifi_umac_hdr *)msg)->ids.nrf_wifi_wiphy_idx;
		wiphy_idx_set = 1;
	}

	
	dl_list_for_each_safe(drv, tmp, &global->interfaces,
			      struct wpa_driver_nl80211_data, list) {
		cfg_do_process_drv_event(drv->first_bss, cmd_evnt, msg, len);
				return NL_SKIP;
#if 0
		for (bss = drv->first_bss; bss; bss = bss->next) {
			if (wiphy_idx_set)
				wiphy_idx = nl80211_get_wiphy_index(bss);
			if ((ifidx == -1 && !wiphy_idx_set && !wdev_id_set) ||
			    ifidx == bss->ifindex ||
			    (wiphy_idx_set && wiphy_idx == wiphy_idx_rx) ||
			    (wdev_id_set && bss->wdev_id_set &&
			     wdev_id == bss->wdev_id)) {
				cfg_do_process_drv_event(bss, cmd_evnt, msg, len);
				return NL_SKIP;
			}
		}
#endif
		wpa_printf(MSG_DEBUG,
			   "nl80211: Ignored event %d (%s) for foreign interface (ifindex %d wdev 0x%llx)",
			   cmd_evnt, nl80211_command_to_string(cmd_evnt),
			   ifidx, (long long unsigned int) wdev_id);

	}

	return NL_SKIP;
}
#endif

int process_global_event(struct nl_msg *msg, void *arg)
{
	struct nl80211_global *global = arg;
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct wpa_driver_nl80211_data *drv, *tmp;
	int ifidx = -1, wiphy_idx = -1, wiphy_idx_rx = -1;
	struct i802_bss *bss;
	u64 wdev_id = 0;
	int wdev_id_set = 0;
	int wiphy_idx_set = 0;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (tb[NL80211_ATTR_IFINDEX])
		ifidx = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);
	else if (tb[NL80211_ATTR_WDEV]) {
		wdev_id = nla_get_u64(tb[NL80211_ATTR_WDEV]);
		wdev_id_set = 1;
	} else if (tb[NL80211_ATTR_WIPHY]) {
		wiphy_idx_rx = nla_get_u32(tb[NL80211_ATTR_WIPHY]);
		wiphy_idx_set = 1;
	}

	dl_list_for_each_safe(drv, tmp, &global->interfaces,
			      struct wpa_driver_nl80211_data, list) {
		for (bss = drv->first_bss; bss; bss = bss->next) {
			if (wiphy_idx_set)
				wiphy_idx = nl80211_get_wiphy_index(bss);
			if ((ifidx == -1 && !wiphy_idx_set && !wdev_id_set) ||
			    ifidx == bss->ifindex ||
			    (wiphy_idx_set && wiphy_idx == wiphy_idx_rx) ||
			    (wdev_id_set && bss->wdev_id_set &&
			     wdev_id == bss->wdev_id)) {
				do_process_drv_event(bss, gnlh->cmd, tb);
				return NL_SKIP;
			}
		}
		wpa_printf(MSG_DEBUG,
			   "nl80211: Ignored event %d (%s) for foreign interface (ifindex %d wdev 0x%llx)",
			   gnlh->cmd, nl80211_command_to_string(gnlh->cmd),
			   ifidx, (long long unsigned int) wdev_id);
	}

	return NL_SKIP;
}


int process_bss_event(struct nl_msg *msg, void *arg)
{
	struct i802_bss *bss = arg;
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	wpa_printf(MSG_DEBUG, "nl80211: BSS Event %d (%s) received for %s",
		   gnlh->cmd, nl80211_command_to_string(gnlh->cmd),
		   bss->ifname);

	switch (gnlh->cmd) {
	case NL80211_CMD_FRAME:
	case NL80211_CMD_FRAME_TX_STATUS:
		mlme_event(bss, gnlh->cmd, tb[NL80211_ATTR_FRAME],
			   tb[NL80211_ATTR_MAC], tb[NL80211_ATTR_TIMED_OUT],
			   tb[NL80211_ATTR_WIPHY_FREQ], tb[NL80211_ATTR_ACK],
			   tb[NL80211_ATTR_COOKIE],
			   tb[NL80211_ATTR_RX_SIGNAL_DBM],
			   tb[NL80211_ATTR_STA_WME], NULL);
		break;
	case NL80211_CMD_UNEXPECTED_FRAME:
		nl80211_spurious_frame(bss, tb, 0);
		break;
	case NL80211_CMD_UNEXPECTED_4ADDR_FRAME:
		nl80211_spurious_frame(bss, tb, 1);
		break;
	case NL80211_CMD_EXTERNAL_AUTH:
		nl80211_external_auth(bss->drv, tb);
		break;
	case NL80211_CMD_CONTROL_PORT_FRAME:
		nl80211_control_port_frame(bss->drv, tb);
		break;
	default:
		wpa_printf(MSG_DEBUG, "nl80211: Ignored unknown event "
			   "(cmd=%d)", gnlh->cmd);
		break;
	}

	return NL_SKIP;
}
