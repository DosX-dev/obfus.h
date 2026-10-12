import { LEVEL_LABELS } from '../core/config.js';
const escapeAttribute = value => String(value).replace(/[&<>"']/g, character => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[character]));
export function description(state) {
    const links = [['easy', '🟢', 'EASY'], ['medium', '🟠', 'MEDIUM'], ['hardcore', '🔴', 'HARDCORE'], ['extreme', '🟣', 'EXTREME']]
        .filter(([level]) => state.files.some(file => file.role === level))
        .map(([level, emoji, label]) => {
            const file = state.files.find(file => file.role === level);
            return `<td align="center"><h2><a href="${escapeAttribute(file.url)}">${emoji} ${label}</a></h2></td>`;
        }).join('\n');
    const answers = state.previous ? `\n\n<details>\n<summary>Previous round answers · Round ${state.previous.round}</summary>\n\n${Object.entries(state.previous.answers).map(([level, password]) => `- **${LEVEL_LABELS[level]}**: \`${password}\``).join('\n')}\n\n</details>` : '';
    const source = /^[a-f0-9]{40,64}$/.test(state.commit || '') ? ` · [Build source](https://github.com/DosX-dev/obfus.h/tree/${state.commit})` : '';
    return `Round **${state.round}** · **${state.date}** · Windows x64${source}\n\n## Download task\n\n<table>\n<tr>\n${links}\n</tr>\n</table>\n\nRecover the password. Patching does not count.\n\nNew round daily at **13:00 Moscow time (UTC+3)**. Save your downloads.${answers}\n`;
}
